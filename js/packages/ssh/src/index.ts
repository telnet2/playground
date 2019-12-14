/**
 * Create mongo conf and service files to launch mongod.
 *
 */
import _ = require("lodash");
import path = require("path");
import fs = require("fs");
import util = require("util");
import njk = require("nunjucks");
import sshConfig = require("ssh-config");
import { Client } from "ssh2";
import * as debugLib from "debug";

declare module "ssh2-streams" {
    class SFTPStream {
        createFile(remotePath: string, content: string): Promise<void>;
        asyncExists(path: string): Promise<boolean>;
        asyncEnsureDir(path: string): Promise<void>;
        asyncEnsureDirp(path: string): Promise<void>;
    }
}

declare module "ssh2" {
    class Client {
        asyncShell(): Promise<Channel>;
    }
}

const debug_ssh = debugLib("ssh:debug");
const debug = debugLib("ssh");
const config = sshConfig.parse(fs.readFileSync(`${process.env["HOME"]}/.ssh/config`).toString());

njk.configure({ autoescape: true });

export type Host = {
    alias?: string;
    host?: string;
    user?: string;
    keyFile?: string;
};

export class SshClient {
    info: Host;
    constructor(info: Host) {
        this.info = info;
    }
}

// Open a sftp session from SSH client.
// Attach createFile, asyncExists, asyncEnsureDirp functions.
function asyncSftp(ssh) {
    return new Promise((resolve, reject) => {
        ssh.sftp((err, sftp) => {
            if (err) {
                reject(err);
            } else {
                ["mkdir", "chown", "writeFile", "readFile"].forEach(fn => {
                    sftp[fn] = util.promisify(sftp[fn]).bind(sftp);
                });

                sftp.createFile = async function(remotePath, txt) {
                    try {
                        await sftp.asyncEnsureDirp(path.dirname(remotePath));
                        await sftp.writeFile(remotePath, txt);
                        console.log(`Wrote ${remotePath}`);
                    } catch (err) {
                        console.error("CreateFile Error:", remotePath, err.message);
                    }
                };

                sftp.asyncExists = async function asyncExists(path) {
                    return new Promise(resolve => {
                        sftp.exists(path, ret => {
                            resolve(ret);
                        });
                    });
                };

                sftp.asyncEnsureDir = async function asyncEnsureDir(dir) {
                    if (!(await sftp.asyncExists(dir))) {
                        let ok = await sftp.mkdir(dir);
                        debug("mkdir %s ... [%s]", dir, ok);
                    }
                };

                sftp.asyncEnsureDirp = async function asyncEnsureDirp(dir) {
                    const dirs = dir.split("/");
                    for await (let i of _.range(1, dirs.length)) {
                        let p = dirs.slice(0, i + 1).join("/");
                        await sftp.asyncEnsureDir(p);
                    }
                };

                resolve(sftp);
            }
        });
    });
}

/**
 * Create a SSH connection to host.
 * The host information is obtained from .ssh/config file.
 * If the config file doesn't exist, hosts.keyFile is used.
 */
function connect(host) {
    let hostInfo = config.compute(host.name);

    if (_.isEmpty(hostInfo)) {
        hostInfo = {
            Host: host.name,
            HostName: host.name,
            IdentityFile: host.keyFile
        };
    }

    const ssh = new Client();
    const keyFile = hostInfo.IdentityFile.toString();

    if (!fs.existsSync(keyFile)) {
        console.error(`Can't read ${keyFile}`);
        return;
    }

    return new Promise((resolve, reject) => {
        const conx = {
            host: hostInfo.HostName,
            user: hostInfo.User,
            privateKey: fs.readFileSync(keyFile),
            debug: debug_ssh
        };
        console.log(`[${host.serviceName}]`, "connecting to ", JSON.stringify(_.pick(conx, "host", "user")));

        ssh.asyncShell = function asyncShell() {
            return new Promise((resolve, reject) => {
                ssh.shell((err, stream) => {
                    if (err) reject(err);
                    else resolve(stream);
                });
            });
        };

        ssh.asyncSftp = () => asyncSftp(ssh);

        // ssh.once("keyboard-interactive", (name, instructions, instructionsLang, prompts, finish) => {
        // console.log(name);
        // console.log(instructions);
        // console.log(instructionsLang);
        // console.log(prompts);
        // finish();
        // });
        ssh.command = function command(cmd, opts = {}) {
            return new Promise((resolve, reject) => {
                let cont = false;
                cont = ssh.exec(cmd, opts, (err, stream) => {
                    if (err) reject(err);
                    else {
                        if (cont) {
                            if (opts.pipe === true) {
                                stream.pipe(process.stdout);
                                stream.stderr.pipe(process.stderr);
                                debug("waiting for stream end: %s", cmd);
                                stream.once("end", () => resolve());
                            } else if (!!opts.pipe) {
                                stream.pipe(opts.pipe);
                                if (!!opts.errPipe) {
                                    stream.stderr.pipe(opts.errPipe);
                                }
                                stream.once("end", () => resolve());
                            } else {
                                resolve(stream);
                            }
                        } else {
                            console.log("wait for continue...");
                            ssh.once("continue", () => {
                                resolve(stream);
                            });
                        }
                    }
                });
            });
        };

        ssh.finish = function finish() {
            return new Promise((resolve, reject) => {
                ssh.once("err", reject);
                ssh.once("close", resolve);
                ssh.end();
            });
        };
        ssh.on("error", reject);
        ssh.on("ready", () => resolve(ssh));
        ssh.connect(conx);
    });
}

/**
 * Create a mongo setup directory on remote.
 *
 */
async function setupMongo(cmds) {
    const { file, install, dryrun, scripts } = cmds;
    const data = require(file);
    console.log("loading", file, data);

    const { hosts, ...common } = data;

    let hostId = hosts.length;
    for await (let host of _.reverse(hosts)) {
        // We copy common properties to the host object.
        const data = Object.assign(host, common);
        data.hostId = --hostId;
        data.replSet = hosts;

        const ssh = await connect(data);
        ssh.once("banner", console.log);
        ssh.once("close", () => console.log("closed connection to:", host.name));

        if (dryrun) {
            console.log(`${data.mongoHome}/conf/createUsers.js`);
            console.log(njk.render("./createUsers.js.njk", data));

            console.log(`${data.mongoHome}/conf/mongod.init.sh`);
            console.log(njk.render("./mongod.init.njk", data));

            if (data.hostId === 0) {
                console.log(`${data.mongoHome}/conf/rsInit.js`);
                console.log(njk.render("./rsInit.js.njk", data));

                console.log(`${data.mongoHome}/conf/rsInit.sh`);
                console.log(njk.render("./rsInit.sh.njk", data));
            }

            await ssh.finish();
            continue;
        } else if (install) {
            const sftp = await ssh.asyncSftp();
            try {
                await sftp.asyncEnsureDirp(`${data.mongoHome}/logs`);
                await sftp.asyncEnsureDirp(`${data.mongoHome}/data`);
                await sftp.createFile(`${data.mongoHome}/conf/mongod.conf`, njk.render("./mongod.conf.njk", data));
                await sftp.createFile(
                    `${data.mongoHome}/conf/disable-transparent-hugepages`,
                    fs.readFileSync("./disable-transparent-hugepages")
                );
                await sftp.createFile(`${data.mongoHome}/conf/mongo-rs.key`, fs.readFileSync("./mongo-rs.key"));
                await sftp.createFile(
                    `${data.mongoHome}/conf/mongod_${data.serviceName}.service`,
                    njk.render("./mongod.service.njk", data)
                );
                await sftp.createFile(
                    `${data.mongoHome}/conf/createUsers.js`,
                    njk.render("./createUsers.js.njk", data)
                );
                await sftp.createFile(`${data.mongoHome}/conf/mongod.init.sh`, njk.render("./mongod.init.njk", data));

                if (data.hostId === 0) {
                    await sftp.createFile(`${data.mongoHome}/conf/rsInit.sh`, njk.render("./rsInit.sh.njk", data));
                    await sftp.createFile(`${data.mongoHome}/conf/rsInit.js`, njk.render("./rsInit.js.njk", data));
                }
            } catch (err) {
                console.error(err);
            } finally {
                sftp.end();
            }
        }

        if (!dryrun && scripts) {
            await ssh.command(`cd ${data.mongoHome}/conf && bash mongod.init.sh`, { pipe: true });
            if (data.hostId === 0) {
                await ssh.command(`cd ${data.mongoHome}/conf && bash rsInit.sh`, { pipe: true });
            }
        }

        await ssh.finish();
    }
}

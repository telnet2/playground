"use strict";
var __asyncValues = (this && this.__asyncValues) || function (o) {
    if (!Symbol.asyncIterator) throw new TypeError("Symbol.asyncIterator is not defined.");
    var m = o[Symbol.asyncIterator], i;
    return m ? m.call(o) : (o = typeof __values === "function" ? __values(o) : o[Symbol.iterator](), i = {}, verb("next"), verb("throw"), verb("return"), i[Symbol.asyncIterator] = function () { return this; }, i);
    function verb(n) { i[n] = o[n] && function (v) { return new Promise(function (resolve, reject) { v = o[n](v), settle(resolve, reject, v.done, v.value); }); }; }
    function settle(resolve, reject, d, v) { Promise.resolve(v).then(function(v) { resolve({ value: v, done: d }); }, reject); }
};
var __rest = (this && this.__rest) || function (s, e) {
    var t = {};
    for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p) && e.indexOf(p) < 0)
        t[p] = s[p];
    if (s != null && typeof Object.getOwnPropertySymbols === "function")
        for (var i = 0, p = Object.getOwnPropertySymbols(s); i < p.length; i++) {
            if (e.indexOf(p[i]) < 0 && Object.prototype.propertyIsEnumerable.call(s, p[i]))
                t[p[i]] = s[p[i]];
        }
    return t;
};
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Create mongo conf and service files to launch mongod.
 *
 */
const _ = require("lodash");
const path = require("path");
const fs = require("fs");
const util = require("util");
const njk = require("nunjucks");
const sshConfig = require("ssh-config");
const ssh2_1 = require("ssh2");
const debugLib = require("debug");
const debug_ssh = debugLib("ssh:debug");
const debug = debugLib("ssh");
const config = sshConfig.parse(fs.readFileSync(`${process.env["HOME"]}/.ssh/config`).toString());
njk.configure({ autoescape: true });
class SshClient {
    constructor(info) {
        this.info = info;
    }
}
exports.SshClient = SshClient;
// Open a sftp session from SSH client.
// Attach createFile, asyncExists, asyncEnsureDirp functions.
function asyncSftp(ssh) {
    return new Promise((resolve, reject) => {
        ssh.sftp((err, sftp) => {
            if (err) {
                reject(err);
            }
            else {
                ["mkdir", "chown", "writeFile", "readFile"].forEach(fn => {
                    sftp[fn] = util.promisify(sftp[fn]).bind(sftp);
                });
                sftp.createFile = async function (remotePath, txt) {
                    try {
                        await sftp.asyncEnsureDirp(path.dirname(remotePath));
                        await sftp.writeFile(remotePath, txt);
                        console.log(`Wrote ${remotePath}`);
                    }
                    catch (err) {
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
                    var e_1, _a;
                    const dirs = dir.split("/");
                    try {
                        for (var _b = __asyncValues(_.range(1, dirs.length)), _c; _c = await _b.next(), !_c.done;) {
                            let i = _c.value;
                            let p = dirs.slice(0, i + 1).join("/");
                            await sftp.asyncEnsureDir(p);
                        }
                    }
                    catch (e_1_1) { e_1 = { error: e_1_1 }; }
                    finally {
                        try {
                            if (_c && !_c.done && (_a = _b.return)) await _a.call(_b);
                        }
                        finally { if (e_1) throw e_1.error; }
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
    const ssh = new ssh2_1.Client();
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
                    if (err)
                        reject(err);
                    else
                        resolve(stream);
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
                    if (err)
                        reject(err);
                    else {
                        if (cont) {
                            if (opts.pipe === true) {
                                stream.pipe(process.stdout);
                                stream.stderr.pipe(process.stderr);
                                debug("waiting for stream end: %s", cmd);
                                stream.once("end", () => resolve());
                            }
                            else if (!!opts.pipe) {
                                stream.pipe(opts.pipe);
                                if (!!opts.errPipe) {
                                    stream.stderr.pipe(opts.errPipe);
                                }
                                stream.once("end", () => resolve());
                            }
                            else {
                                resolve(stream);
                            }
                        }
                        else {
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
    var e_2, _a;
    const { file, install, dryrun, scripts } = cmds;
    const data = require(file);
    console.log("loading", file, data);
    const { hosts } = data, common = __rest(data, ["hosts"]);
    let hostId = hosts.length;
    try {
        for (var _b = __asyncValues(_.reverse(hosts)), _c; _c = await _b.next(), !_c.done;) {
            let host = _c.value;
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
            }
            else if (install) {
                const sftp = await ssh.asyncSftp();
                try {
                    await sftp.asyncEnsureDirp(`${data.mongoHome}/logs`);
                    await sftp.asyncEnsureDirp(`${data.mongoHome}/data`);
                    await sftp.createFile(`${data.mongoHome}/conf/mongod.conf`, njk.render("./mongod.conf.njk", data));
                    await sftp.createFile(`${data.mongoHome}/conf/disable-transparent-hugepages`, fs.readFileSync("./disable-transparent-hugepages"));
                    await sftp.createFile(`${data.mongoHome}/conf/mongo-rs.key`, fs.readFileSync("./mongo-rs.key"));
                    await sftp.createFile(`${data.mongoHome}/conf/mongod_${data.serviceName}.service`, njk.render("./mongod.service.njk", data));
                    await sftp.createFile(`${data.mongoHome}/conf/createUsers.js`, njk.render("./createUsers.js.njk", data));
                    await sftp.createFile(`${data.mongoHome}/conf/mongod.init.sh`, njk.render("./mongod.init.njk", data));
                    if (data.hostId === 0) {
                        await sftp.createFile(`${data.mongoHome}/conf/rsInit.sh`, njk.render("./rsInit.sh.njk", data));
                        await sftp.createFile(`${data.mongoHome}/conf/rsInit.js`, njk.render("./rsInit.js.njk", data));
                    }
                }
                catch (err) {
                    console.error(err);
                }
                finally {
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
    catch (e_2_1) { e_2 = { error: e_2_1 }; }
    finally {
        try {
            if (_c && !_c.done && (_a = _b.return)) await _a.call(_b);
        }
        finally { if (e_2) throw e_2.error; }
    }
}
//# sourceMappingURL=index.js.map
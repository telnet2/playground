#!/usr/bin/env node

const njk = require("nunjucks");
const fs = require("fs");
const args = require("minimist")(process.argv.slice(2));

njk.configure({ autoescape: true });

function main() {
    if (args._.length < 2) {
        console.error("Usage: input.njk data.js");
        return;
    }
    let json = require(args._[1]);
    let tmpl = fs.readFileSync(args._[0]).toString();

    console.log(njk.render(args._[0], json));
}

if (require.main === module) {
    main();
}

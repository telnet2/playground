// db SH region
module.exports = {
    replSetName: "db-rs",
    adminUser: "admin",
    adminPass: "adminPass",
    dbList: [
        {
            name: "db",
            user: "shell-user",
            pass: "password"
        }
    ],
    hosts: [
        {
            serviceName: "db_rs0",
            name: "hostname",
            user: "shell-user",
            group: "shell-user",
            keyFile: "",
            serverIpAddr: "172.31.20.96",
            mongoHome: `/home/shell-user/data/mongo-db-rs0`,
            mongoPort: 6388
        },
        {
            serviceName: "db_rs1",
            name: "hostname",
            user: "shell-user",
            group: "shell-user",
            keyFile: "",
            serverIpAddr: "172.31.20.96",
            mongoHome: `/home/shell-user/data/mongo-db-rs1`,
            mongoPort: 6389
        }
    ]
};

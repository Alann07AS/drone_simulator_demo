

mn.insert(document.currentScript, (updater, oldupdater) => {
    return [!data.log ?
        mn.element.create(
            "a",
            {
                href: "#/login"
            },
            "Login"
        ) :
        mn.element.create(
            "a",
            {
                href: "#/unlog"
            },
            "Unlog: " + data.log
        )]
})
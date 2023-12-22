
(() => {
    const scriptcoord = document.currentScript
    const i = scriptcoord.getAttribute("drone_index")
    
    
    if (!data.drones[i]) return
    mn.insert(scriptcoord, (up, oldup) => {
        mn.data.bind("drones", up)
        return [
            mn.element.create(
                "div",
                {
                    class: "coords",
                },
                mn.element.create(
                    "p",
                    {},
                    data.drones[i].name
                ),
                mn.element.create(
                    "p",
                    {},
                    data.drones[i].longitude
                ),
                mn.element.create(
                    "p",
                    {},
                    data.drones[i].latitude
                )
            )
        ]
    })
})()
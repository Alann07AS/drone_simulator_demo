


const script_drones = document.currentScript;
let upd;
const markers = [];

mn.insert(script_drones, (updater, oldupdater) => {

    if (!data.drones) {
        upd = updater
        mn.data.bind("drones", upd)
    } else {
        mn.data.remove_bind("drones", upd);
    }

    mn.data.bind("drones", oldupdater((els => {
        if (!data.drones) return
        els.forEach((el, id) => {
            markers[id].setLatLng([data.drones[id].latitude, data.drones[id].longitude]);
            el.querySelector(".dronename").textContent = data.drones[id].name
            el.querySelector(".dronespeed").textContent = "Speed: " + (data.drones[id].speed ? data.drones[id].speed - 100 : 0) + " km/h"
            el.querySelector(".dronealt").textContent = "Altitude: " + data.drones[id].altitude + "m"

        });
    })))
    if (!data.drones) return []
    return data.drones.map(drone => {
        markers.push(L.marker([drone.latitude, drone.longitude], { icon: droneIcon }).addTo(map))
        markers.at(-1).bindPopup(
                mn.element.create(
                "script",
                {
                    src: "./elements/coord.js",
                    drone_index: 0,
                }
            )
        )
        return mn.element.create(
            "div",
            {
                class: "drone border",
                onclick: () => {
                    location.hash = ""
                    location.hash = "#/drone_" + drone.name
                }
            },
            mn.element.create(
                "div",
                {
                    class: "dronename"
                },
                drone.name
            ),
            mn.element.create("div", { class: "vert_line" }),
            mn.element.create(
                "div",
                {
                    class: "dronespeed"
                },
                "Speed: " + (drone.speed ? drone.speed - 100 : 0) + " km/h"
            ),
            mn.element.create("div", { class: "vert_line" }),
            mn.element.create(
                "div",
                {
                    class: "dronealt",
                    title: "*Au dessus de la mer."
                },
                "Altitude: " + drone.altitude + "m"
            ),
        )
    })
})
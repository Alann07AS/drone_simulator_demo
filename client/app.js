// const api_adresse = "https://192.168.101.44/"
const api_adresse = "https://delivroneesp32demo.duckdns.org:20020/"

let map

var droneIcon = L.icon({
    iconUrl: 'drone.ico',
    iconSize: [60, 75],
    iconAnchor: [30, 75],
    popupAnchor: [0, 0],
    // shadowUrl: 'my-icon-shadow.png',
    // shadowSize: [68, 95],
    // shadowAnchor: [22, 94]
});

function getCookie(name) {
    const value = `; ${document.cookie}`;
    const parts = value.split(`; ${name}=`);
    if (parts.length === 2) return parts.pop().split(';').shift();
}
function deleteAllCookies() {
    const cookies = document.cookie.split(";");
    for (let i = 0; i < cookies.length; i++) {
        const cookie = cookies[i];
        const eqPos = cookie.indexOf("=");
        const name = eqPos > -1 ? cookie.substr(0, eqPos) : cookie;
        document.cookie = name + "=;expires=Thu, 01 Jan 1970 00:00:00 GMT";
    }
}

function updateDrones() {
    req("drones")
        .then(r => r.headers.get("content-type") === "application/json" ? r.json() : r)
        .then(drones => {
            console.log(drones);
            data.drones = drones.drones;
            mn.data.update("drones")
            updateDrones();
        })
        .catch(_ => updateDrones())
}

function updateOrders() {
    req("orders")
        .then(r => r.headers.get("content-type") === "application/json" ? r.json() : r)
        .then(orders => {
            console.log(orders);
            data.orders = orders.orders;
            mn.data.update("orders")
            updateOrders();
        })
        .catch(_ => updateOrders())
}

const req = (endpoint, form) => {
    const data = new URLSearchParams();
    if (form) {
        for (const pair of new FormData(form)) {
            data.append(pair[0], pair[1]);
        }
    }
    return fetch(api_adresse + endpoint, {
        method: form ? "post" : "get",
        credentials: "include",
        body: form ? data : undefined,
    })

}

const login_h = () => mn.id("login_form").style.display = window.location.hash === "#/login" ? "" : "none";
const unlog_h = () => {
    if (window.location.hash !== "#/unlog") return
    console.log("unlog");
    deleteAllCookies();
    document.location.hash = "";
    delete data.log;
    saveData()
    document.location.reload();
};
const focusDrone = () => {
    const hash = window.location.hash.split("_")
    if (hash[0] != "#/drone") return
    const id = data.drones.findIndex(d => d.name === hash[1])
    map.panTo(markers[id].getLatLng())
}


window.onload = () => {
    updateDrones()
    updateOrders()

    login_h()
    window.addEventListener("hashchange", () => {
        login_h()
        unlog_h()
        focusDrone()
    })

    mn.id("login_form").onsubmit = (e) => {
        e.preventDefault();
        req("login", mn.id("login_form"))
            .then(r => {
                console.log(r.headers.getSetCookie());
                if (r.headers.get("content-type") === "application/json") {
                    r.json()
                        .then(obj => {
                            console.log(obj);
                            mn.data.set("log", obj.log);
                            console.log(data);
                            saveData()
                            console.log(document.cookie);
                            const cookieString = `${obj.UUID}=YourCookieValue; SameSite=None; Secure; HttpOnly`;
                            document.cookie = cookieString;
                            console.log(document.cookie);
                            window.location.hash = "";
                            document.location.reload();
                        })
                } else {
                    console.error("login contenttype error");
                }
                if (r.headers.has('Set-Cookie')) {
                    // Get the cookie value from the Set-Cookie header
                    const cookieValue = r.headers.get('Set-Cookie');

                    // Set the cookie in the document
                    document.cookie = cookieValue;
                }
            })
            .catch(e => console.log("log error: ", e))
    }

    mn.id("takeorder").onsubmit = (e) => {
        e.preventDefault();
        req("order", mn.id("takeorder"))
            .then(r => {
                console.log(r);
                console.log(r.headers.getSetCookie());
                if (r.headers.get("content-type") === "application/json") {
                    r.json()
                        .then(obj => {
                            console.log(obj);
                        })
                } else {
                    console.log(r);
                }
            })
            .catch(e => console.log("log error: ", e))
    }

}


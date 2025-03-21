class DataServer {
    constructor() {
        if (!DataServer.instance) {
            this.XHR = new XMLHttpRequest();
            this.ws = null;
            this.updateTimeout = null; // Prevent multiple timers
            DataServer.instance = this;
        }
        return DataServer.instance;
    }

    static update(timeout = 5000) {
        const instance = new DataServer();
        let xhr = instance.XHR;

        if (xhr.readyState !== 0 && xhr.readyState !== 4) {
            xhr.abort();
        }

        try {
            xhr.open("GET", "/data-server", true);

            xhr.onload = () => {
                if (xhr.status === 200) {
                    let response = JSON.parse(xhr.responseText);
                    let data_server = response.data_server;
                    let data_relay = response.data_relay;

                    const updateElement = (id, value) => {
                        const elem = document.getElementById(id);
                        if (elem && elem.innerHTML !== value) elem.innerHTML = value || "N/A";
                    };

                    updateElement("server-temp", `${data_server?.dht?.temp}°C`);
                    updateElement("server-hum", `${data_server?.dht?.hum}%`);
                    updateElement("server-soillvl", `${data_server?.soil_moisture}%`);
                    updateElement(
                        "server-WateringState",
                        data_server?.watering_state ? "Watering" : "Standby"
                    );

                    for (let key in data_relay) {
                        let element = document.getElementById(data_relay[key].id);
                        if (element) {
                            element.checked = data_relay[key].status;
                        }
                    }
                } else {
                    console.error("Error fetching data:", xhr.statusText);
                }
            };

            xhr.onerror = () => console.error("XHR request failed.");

            xhr.send();
        } catch (error) {
            console.error("XHR error:", error);
        }

        // Cegah multiple timeout dengan clearTimeout
        clearTimeout(instance.updateTimeout);
        instance.updateTimeout = setTimeout(() => DataServer.update(timeout), timeout);
    }

    static listen() {
        const instance = new DataServer();

        // Hindari banyak WebSocket
        if (instance.ws && instance.ws.readyState === WebSocket.OPEN) {
            console.warn("WebSocket already connected.");
            return;
        }

        instance.ws = new WebSocket(`ws://${window.location.host}/ws`);

        instance.ws.onopen = () => {
            console.log("WebSocket Connected!");
            instance.ws.send(JSON.stringify({ event: "data_server" }));
        };

        instance.ws.onmessage = (event) => {
            try {
                const data = JSON.parse(event.data);
                if (data.event === "data_server") {
                    let data_server = data.data_server;

                    const updateElement = (id, value) => {
                        const elem = document.getElementById(id);
                        if (elem && elem.innerHTML !== value) elem.innerHTML = value || "N/A";
                    };

                    updateElement("server-temp", `${data_server?.dht?.temp}°C`);
                    updateElement("server-hum", `${data_server?.dht?.hum}%`);
                    updateElement("server-soillvl", `${data_server?.soil_moisture}%`);
                    updateElement(
                        "server-WateringState",
                        data_server?.watering_state ? "Watering" : "Standby"
                    );

                    let data_relay = data.data_relay;
                    for (let key in data_relay) {
                        let element = document.getElementById(data_relay[key].id);
                        if (element) {
                            element.checked = data_relay[key].status;
                        }
                    }
                }
            } catch (error) {
                console.error("WebSocket data parsing error:", error);
            }
        };

        instance.ws.onclose = () => {
            console.warn("WebSocket closed, reconnecting...");
            setTimeout(() => DataServer.listen(), 5000);
        };

        instance.ws.onerror = (error) => {
            console.error("WebSocket error:", error);
            instance.ws.close();
        };
    }
}

// Mulai otomatis saat halaman dimuat
document.addEventListener("DOMContentLoaded", () => {
    DataServer.update(1500);
    DataServer.listen();
});

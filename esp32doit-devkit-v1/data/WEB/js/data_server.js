class DataServer {
    constructor() {
        this.XHR = new XMLHttpRequest();
        this.ws = null;
    }

    static update(timeout) {
        const __dataServer = new DataServer();
        let xhr = __dataServer.XHR;

        try {
            xhr.abort();
            xhr.open("GET", "/data-server", true);
            xhr.onreadystatechange = () => {
                if (xhr.readyState === 4) {
                    if (xhr.status === 200) {
                        let response = JSON.parse(xhr.responseText);
                        // let heap_memory = response.heap_memory;
                        let data_server = response.data_server;
                        let data_relay = response.data_relay;

                        // Reload the page if free heap memory is critically low
                        // if (heap_memory?.free_heap && heap_memory.free_heap <= 20.00) {
                        //     window.location.reload();
                        // }

                        // Update DOM elements only if they exist
                        const updateElement = (id, value) => {
                            const elem = document.getElementById(id);
                            if (elem) elem.innerHTML = value || "N/A";
                        };

                        // updateElement("server-free_heap", heap_memory?.free_heap);
                        // updateElement("server-total_heap", heap_memory?.total_heap);
                        // updateElement("server-date", data_server?.date);
                        // updateElement("server-time", data_server?.time);
                        updateElement("server-temp", data_server?.dht?.temp);
                        updateElement("server-hum", data_server?.dht?.hum);
                        updateElement("server-soillvl", data_server?.soil_moisture);
                        updateElement(
                            "server-WateringState",
                            data_server?.WateringState ? "Watering" : "Standby"
                        );
                        // updateElement("server-ScheduleWatering", data_server?.ScheduleWatering);

                        // Update relay statuses
                        for (let key in data_relay) {
                            let element = document.getElementById(data_relay[key].id);
                            if (element) {
                                element.checked = data_relay[key].status;
                            }
                        }
                    } else {
                        console.error("Error fetching data:", xhr.statusText);
                    }
                }
            };
            xhr.send();
        } catch (error) {
            console.error("XHR error:", error);
        }

        // Safe timeout to prevent overlapping
        setTimeout(() => DataServer.update(timeout), timeout);
    }

    static listen() {
        if (this.ws && this.ws.readyState === WebSocket.OPEN) {
            return;
        }

        this.ws = new WebSocket(`ws://${window.location.host}/ws`);
        this.ws.onopen = () => {
            this.ws.send(JSON.stringify({ event: "data_server" }));
        };
        this.ws.onmessage = (event) => {
            const data = JSON.parse(event.data);
            let heap_memory = data.heap_memory;
                
            if (data?.free_heap && data?.free_heap <= 20.00) {
                window.location.reload();
            }

            if (data.event === "data_server") {
                let data_server = data.data_server;
                
                const updateElement = (id, value) => {
                    const elem = document.getElementById(id);
                    if (elem) elem.innerHTML = value || "N/A";
                };

                // updateElement("server-free_heap", heap_memory?.free_heap);
                // updateElement("server-total_heap", heap_memory?.total_heap);
                // updateElement("server-date", data_server?.date);
                // updateElement("server-time", data_server?.time);
                updateElement("server-temp", data_server?.dht?.temp);
                updateElement("server-hum", data_server?.dht?.hum);
                updateElement("server-soillvl", data_server?.soil_moisture);
                updateElement(
                    "server-WateringState",
                    data_server?.WateringState ? "Watering" : "Standby"
                );
                // updateElement("server-ScheduleWatering", data_server?.ScheduleWatering);

                let data_relay = data.data_relay;
                for (let key in data_relay) {
                    let element = document.getElementById(data_relay[key].id);
                    if (element) {
                        element.checked = data_relay[key].status;
                    }
                }
            }
        };

        this.ws.onclose = () => {
            console.warn("WebSocket closed, reconnecting...");
            setTimeout(() => DataServer.listen(), 3000);
        };
        this.ws.onerror = (error) => {
            console.error("WebSocket error:", error);
            this.ws.close();
        };
    }
}

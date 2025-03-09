class DateTimeRTC {
    constructor() {
        this.XHR = new XMLHttpRequest();
        this.ws = null;
    }

    static update(timeout) {
        const __dataServer = new DateTimeRTC();
        let xhr = __dataServer.XHR;

        try {
            xhr.abort();
            xhr.open("GET", "/datetime", true);
            xhr.onreadystatechange = () => {
                if (xhr.readyState === 4) {
                    if (xhr.status === 200) {
                        let response = JSON.parse(xhr.responseText);
                        let datetimertc = response.datetime;
                        
                        // Update DOM elements only if they exist
                        const updateElement = (id, value) => {
                            const elem = document.getElementById(id);
                            if (elem) elem.innerHTML = value || "N/A";
                        };

                        updateElement("server-date", datetimertc?.date);
                        updateElement("server-time", datetimertc?.time);
                        
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
        setTimeout(() => DateTimeRTC.update(timeout), timeout);
    }

    static listen() {
        this.ws = new WebSocket(`ws://${window.location.host}/ws`);
        this.ws.onopen = () => {
            this.ws.send(JSON.stringify({ event: "datetime" }));
        };
        this.ws.onmessage = (event) => {
            const data = JSON.parse(event.data);
            let heap_memory = data.heap_memory;
            if (heap_memory?.free_heap && heap_memory?.free_heap <= 20.0) {
                window.location.reload();
            }

            if (data.event === "datetime") {
                let datetime = data.datetime;
                
                const updateElement = (id, value) => {
                    const elem = document.getElementById(id);
                    if (elem) elem.innerHTML = value || "N/A";
                };

                updateElement("server-date", datetime?.date);
                updateElement("server-time", datetime?.time);
            }
        };

        this.ws.onclose = () => {
            console.warn("WebSocket closed, reconneting...");
            setTimeout(() => DateTimeRTC.listen(), 3000);
        };
        this.ws.onerror = (error) => {
            console.error("WebSocket error : ", error);
            this.ws.close();
        }
    }

}

class toggleCheck {
    constructor() {
        if (!toggleCheck.instance) {
            this._xhr = new XMLHttpRequest();
            toggleCheck.instance = this;
        }
        return toggleCheck.instance;
    }

    static handleError(xhr) {
        xhr.onload = () => {
            if (xhr.status === 400) {
                Swal.fire({
                    title: "Information",
                    text: "Auto Watering is Enabled",
                    icon: "info",
                    confirmButtonColor: "#00b30c",
                    confirmButtonText: "OK"
                });
            } else if (xhr.status !== 200) {
                console.error("Request Error:", xhr.statusText);
            }
        };

        xhr.onerror = () => console.error("XHR request failed.");
    }

    static autoWateringCheckbox(element) {
        let instance = new toggleCheck();
        let xhr = instance._xhr;

        if (xhr.readyState !== 0 && xhr.readyState !== 4) {
            xhr.abort();
        }

        let state = element.checked ? 1 : 0;
        let uri = `/auto-watering?state=${state}`;

        xhr.open("GET", uri, true);
        toggleCheck.handleError(xhr);
        xhr.send();

        Swal.fire({
            title: `Auto Watering: ${state ? "Enabled" : "Disabled"}`,
            icon: "info",
            toast: true,
            position: "top-end",
            showConfirmButton: false,
            timer: 1500
        });
    }

    static toggleCheckbox(element) {
        let instance = new toggleCheck();
        let xhr = instance._xhr;

        if (xhr.readyState !== 0 && xhr.readyState !== 4) {
            xhr.abort();
        }

        let state = element.checked ? 1 : 0;
        let uri = `/check?pinout=${element.id}&state=${state}`;

        xhr.open("GET", uri, true);
        toggleCheck.handleError(xhr);
        xhr.send();
    }
}

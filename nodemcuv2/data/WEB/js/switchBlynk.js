class switchBlynk {
    constructor() {
        this._xhr = new XMLHttpRequest();
    }

    _get(uri) {
        this._xhr.open("GET", uri, true);
        this._xhr.send();
    }

    static run() {
        const thisClass = new switchBlynk();
        Swal.fire({
            title: "Enable Blynk",
            text: "Enable blynk mode?",
            icon: "info",
            showCancelButton: true,
            confirmButtonColor: "#3085d6",
            cancelButtonColor: "#d33",
            confirmButtonText: "Enable"
        }).then((result) => {
            if (result.isConfirmed) {
                thisClass._get("/enable-blynk");
                Swal.fire({
                    title: "Enable Success",
                    text: "Server switch mode Blynk, restarting...",
                    icon: "success",
                    confirmButtonColor: "#00b30c",
                    confirmButtonText: "OK"
                });
            }
        });
    }
}

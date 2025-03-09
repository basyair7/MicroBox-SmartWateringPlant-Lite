class reboot {
    constructor() {
        this._xhr = new XMLHttpRequest();
    }

    _get(uri) {
        this._xhr.open("GET", uri, true);
        this._xhr.send();
    }

    static rebootServer() {
        const thisClass = new reboot();
        Swal.fire({
            title: "Reboot Unit",
            text: "Reboot the web server?",
            icon: "info",
            showCancelButton: true,
            showCancelButton: true,
            confirmButtonColor: "#3085d6",
            cancelButtonColor: "#d33",
            confirmButtonText: "Reboot"
        }).then((result) => {
            if (result.isConfirmed) {
                thisClass._get("/rst-webserver");
                Swal.fire({
                    title: "Reboot Success",
                    text: "Server has been restart",
                    icon: "success",
                    confirmButtonColor: "#00b30c",
                    confirmButtonText: "OK"
                });
            }
        });
    }

}

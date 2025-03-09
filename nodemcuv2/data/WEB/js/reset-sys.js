class reset {
    constructor() {
        this._xhr = new XMLHttpRequest();
    }

    _get(uri) {
        try {    
            this._xhr.open("GET", uri, true);
            this._xhr.send();
        } catch (error) {
            console.error(error);
        }
    }

    static resetSystem(uri) {
        Swal.fire({
            title: "Reset System",
            text: "Reset the system to default?",
            icon: "warning",
            showCancelButton: true,
            confirmButtonColor: "#3085d6",
            cancelButtonColor: "#d33",
            confirmButtonText: "Yes, reset!"
        }).then((result) => {
            if (result.isConfirmed) {
                window.location.href = uri;
            }
        });
    }
}

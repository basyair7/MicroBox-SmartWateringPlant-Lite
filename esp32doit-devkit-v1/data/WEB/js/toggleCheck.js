class toggleCheck {
    constructor() {
        this._xhr = new XMLHttpRequest();
    }
    handlerError(xhr) {
        xhr.onreadystatechange = async() => {
            if (xhr.status === 400) {
                Swal.fire({
                    title: "Information",
                    text: "Auto Watering is Enable",
                    icon: "info",
                    confirmButtonColor: "#00b30c",
                    confirmButtonText: "OK"
                });
            }
        };
    }

    static autoWateringCheckbox(element) {
        var thisClass = new toggleCheck();
        let xhr = thisClass._xhr;
        let uri = (state) => { return "/auto-watering?state=" + state; };

        if (element.checked) {
            xhr.open("GET", uri(1), true);
            Swal.fire({
                title: "Auto Watering : Enable",
                icon: "info"
            });
        }
        else {
            xhr.open("GET", uri(0), true);
            Swal.fire({
                title: "Auto Watering : Disable",
                icon: "info"
            });
        }
        xhr.send();
    }

    static async toggleCheckbox(element) {
        var thisClass = new toggleCheck();
        let xhr = thisClass._xhr;
        let uri = (id, state) => { return "/check?pinout=" + id + "&state=" + state; };

        if (element.checked) {
            xhr.open("GET", uri(element.id, 1), true);
            thisClass.handlerError(xhr);
        }
        else {
            xhr.open("GET", uri(element.id, 0), true);
            thisClass.handlerError(xhr);
        }
        xhr.send();
    }

}

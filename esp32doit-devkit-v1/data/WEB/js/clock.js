class Clock {
    constructor() {
        this.updateClock();
        this.start();
    }

    /**
     * @brief Updates the date and time display in the DOM.
     */
    updateClock() {
        const now = new Date();

        // Get current date
        let day = now.getDate();
        let month = now.getMonth() + 1;
        let year = now.getFullYear();

        // Get current time
        let hours = now.getHours();
        let minutes = now.getMinutes();
        let seconds = now.getSeconds();

        // Determine AM or PM
        let AM_PM = hours < 12 ? "AM" : "PM";

        // Convert to 12-hour format
        hours = hours % 12 || 12;  // Converts 0 to 12 for 12 AM

        // Pad numbers with leading zeros
        hours = String(hours).padStart(2, "0");
        minutes = String(minutes).padStart(2, "0");
        seconds = String(seconds).padStart(2, "0");

        // Update the DOM elements with the formatted time and date
        document.getElementById("time").innerHTML = `${hours}:${minutes}:${seconds} ${AM_PM}`;
        document.getElementById("date").innerHTML = `${month}/${day}/${year}`;
    }

    /**
     * @brief Starts the clock and updates the time every second.
     */
    start() {
        if (!Clock.instance) {
            Clock.instance = setInterval(() => this.updateClock(), 1000);
        }
    }
}

// Run the clock only once after the DOM is fully loaded
document.addEventListener("DOMContentLoaded", () => new Clock());

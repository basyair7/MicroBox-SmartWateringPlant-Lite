class Clock {
    constructor() {
        this.rtclock = new Date();
    }
    
    static run() {
        const thisClass = new Clock();

        // date
        let day = thisClass.rtclock.getDate(); // Get the day of the month (1-31)
        let month = thisClass.rtclock.getMonth() + 1; // Get the month (0-11), so adding 1
        let year = thisClass.rtclock.getFullYear();

        // time
        let hours = thisClass.rtclock.getHours();
        let minute = thisClass.rtclock.getMinutes();
        let seconds = thisClass.rtclock.getSeconds();

        // Add AM or PM System
        let AM_PM = (hours < 12) ? "AM" : "PM";

        // convert the hours component to 12-hour format
        hours = (hours > 12) ? hours - 12 : (hours === 0 ? 12 : hours);  // Handle 12 AM as 12, and 0 hour as 12 in PM

        // pad the hours, minute, and seconds with leading zero
        hours = ("0" + hours).slice(-2);
        minute = ("0" + minute).slice(-2);
        seconds = ("0" + seconds).slice(-2);

        document.getElementById("time").innerHTML = `${hours}:${minute}:${seconds} ${AM_PM}`;
        document.getElementById("date").innerHTML = `${month}/${day}/${year}`;
        
        // Use setInterval instead of setTimeout for continuous updates
        setInterval(Clock.run, 1000); // Update every 1000ms (1 second)
    }
}

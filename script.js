// Specify your ThingSpeak channel ID and field ID
const channelID = '2469286';
// const fieldID = '6';
const apiKey = '9XU79MQ2695GW32J'; // Only if your channel is private

// Array of field IDs
const fieldIDs = [1, 2, 3, 4, 5, 6];

// Define the async function to fetch data
async function fetchData(fieldID) {
    try {
        const url = `https://api.thingspeak.com/channels/${channelID}/fields/${fieldID}/last.json?api_key=${apiKey}`;
        const response = await fetch(url);
        const data = await response.json();
        console.log(`Data fetched for field ${fieldID}:`, data[`field${fieldID}`]);

        // Update the content of the existing div with the corresponding field ID
        const existingDiv = document.querySelector(`.field${fieldID}`);
        if (existingDiv) {
            existingDiv.textContent = data[`field${fieldID}`];
            existingDiv.style.color = 'blue';
            existingDiv.style.fontSize = '18px';
        } else {
            console.error(`Div with class 'field${fieldID}' not found.`);
        }
    } catch (error) {
        console.error(`Error fetching data for field ${fieldID}:`, error);
    }
}

// Define a function to fetch data for each field ID in the array
async function fetchDataForFieldIDs(fieldIDs) {
    for (const fieldID of fieldIDs) {
        await fetchData(fieldID);
    }
}

async function fetchDataForAverages() {
    try {
        // Fetch temperature data
        const temp1 = await fetch(`https://api.thingspeak.com/channels/${channelID}/fields/1/last.json?api_key=${apiKey}`).then(res => res.json());
        const temp3 = await fetch(`https://api.thingspeak.com/channels/${channelID}/fields/3/last.json?api_key=${apiKey}`).then(res => res.json());
        const averageTemperature = (parseFloat(temp1.field1) + parseFloat(temp3.field3)) / 2;

        // Fetch humidity data
        const hum2 = await fetch(`https://api.thingspeak.com/channels/${channelID}/fields/2/last.json?api_key=${apiKey}`).then(res => res.json());
        const hum4 = await fetch(`https://api.thingspeak.com/channels/${channelID}/fields/4/last.json?api_key=${apiKey}`).then(res => res.json());
        const averageHumidity = (parseFloat(hum2.field2) + parseFloat(hum4.field4)) / 2;

        // Update HTML with these averages
        document.querySelector('#avgTemperature').textContent = averageTemperature.toFixed(2);
        document.querySelector('#avgHumidity').textContent = averageHumidity.toFixed(2);
    } catch (error) {
        console.error('Failed to fetch or calculate averages:', error);
    }
}


// Initialize data on page load
window.onload = () => {
    fetchDataForFieldIDs(fieldIDs);
    fetchDataForAverages();
};



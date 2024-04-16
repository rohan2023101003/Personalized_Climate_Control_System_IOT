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

// Call the function to fetch data for each field ID when the window loads
window.onload = () => {
    fetchDataForFieldIDs(fieldIDs);
};


const express = require('express');
const cors = require('cors');
const app = express();
const PORT = 3000;

app.use(cors());
app.use(express.json());
app.use(express.static(__dirname));

// A simple in-memory map database for testing
const mapDatabase = {
    "1": {
        "MapName": "iPad Starter World",
        "CreatorID": 1,
        "Workspace": [
            { "Type": "Cube", "Position": {"X": 0, "Y": 1, "Z": -5}, "Scale": {"X": 2, "Y": 2, "Z": 2}, "Color": "#FF0000" }
        ]
    }
};

// Endpoint 1: The client uses this to fetch a map by its ID
app.get('/api/maps/:id', (req, res) => {
    const map = mapDatabase[req.params.id];
    if (!map) return res.status(404).json({ error: "Map not found" });
    res.json(map);
});

// Endpoint 2: The studio tool uses this to save a map
app.post('/api/maps/save', (req, res) => {
    const newMap = req.body;
    const newId = String(Object.keys(mapDatabase).length + 1);
    mapDatabase[newId] = newMap;
    res.json({ success: true, mapId: newId });
});

app.listen(PORT, () => {
    console.log(`Sandbox API Server running on port ${PORT}`);
});

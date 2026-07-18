//prototype multiplayer replication.

using Godot;
using System;
using System.Collections.Generic;

namespace RogoModel.Engine
{
    public partial class NetworkManager : Node3D
    {
        private const int DefaultPort = 10523; // Custom server communication port
        private const int MaxPlayers = 32;

        // Keep track of active network player sessions
        private Dictionary<int, Node3D> activePlayers = new Dictionary<int, Node3D>();

        public override void _Ready()
        {
            // 1. Intercept startup parameters to see if this executable is running as a Headless Server or Player Client
            string[] args = OS.GetCmdlineArgs();
            bool isServer = false;
            string connectIp = "127.0.0.1";

            foreach (string arg in args)
            {
                if (arg == "--server" || arg == "-headless") isServer = true;
                if (arg.StartsWith("--ip=")) connectIp = arg.Replace("--ip=", "");
            }

            if (isServer)
            {
                StartMultiplayerServer();
            }
            else
            {
                ConnectToMultiplayerServer(connectIp);
            }
        }

        // 2. THE SERVER INITIALIZATION ROUTINE (Runs headless on your backend)
        private void StartMultiplayerServer()
        {
            ENetMultiplayerPeer peer = new ENetMultiplayerPeer();
            Error error = peer.CreateServer(DefaultPort, MaxPlayers);

            if (error != Error.Ok)
            {
                GD.PrintErr($"Multiplayer Server failed to initialize: {error}");
                return;
            }

            Multiplayer.MultiplayerPeer = peer;
            GD.Print($"[SERVER] RogoModel Headless Server listening on port {DefaultPort}...");

            // Listen for players connecting or disconnecting over the internet
            Multiplayer.PeerConnected += OnPlayerConnected;
            Multiplayer.PeerDisconnected += OnPlayerDisconnected;
        }

        // 3. THE CLIENT CONNECTION ROUTINE (Triggered by your website's deep link button)
        private void ConnectToMultiplayerServer(string ipAddress)
        {
            ENetMultiplayerPeer peer = new ENetMultiplayerPeer();
            Error error = peer.CreateClient(ipAddress, DefaultPort);

            if (error != Error.Ok)
            {
                GD.PrintErr($"Client network layer dropped connection: {error}");
                return;
            }

            Multiplayer.MultiplayerPeer = peer;
            GD.Print($"[CLIENT] Connecting to network match at {ipAddress}...");
        }

        // 4. PLAYER ROUTINES AND REPLICATION LOGIC
        private void OnPlayerConnected(long id)
        {
            GD.Print($"[SERVER] Player instance authenticated with Network ID: {id}");

            // Create a primitive 3D capsule instance to represent the player's character body
            MeshInstance3D avatarMesh = new MeshInstance3D();
            avatarMesh.Mesh = new CapsuleMesh();
            avatarMesh.Name = id.ToString();
            
            // Give the player character network authority so their movement keys replicate to everyone else
            avatarMesh.SetMultiplayerAuthority((int)id);

            AddChild(avatarMesh);
            activePlayers.Add((int)id, avatarMesh);
        }

        private void OnPlayerDisconnected(long id)
        {
            GD.Print($"[SERVER] Player session disconnected. Discarding Network ID: {id}");
            if (activePlayers.ContainsKey((int)id))
            {
                activePlayers[(int)id].QueueFree(); // Deletes their character from the world
                activePlayers.Remove((int)id);
            }
        }
    }
}

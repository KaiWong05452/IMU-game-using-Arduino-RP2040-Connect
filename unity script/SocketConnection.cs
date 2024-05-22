using System.Collections;
using System.Collections.Generic;
using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;

public class SocketConnection : MonoBehaviour
{
    private Thread thread;
    private TcpListener listener;
    private TcpClient client;
    private NetworkStream stream;
    private byte[] buffer = new byte[1024];
    
    private float pitch = 0f;
    private float roll = 0f;
    private float yaw = 0f;

    private float initialPitch = 0f;
    private float initialRoll = 0f;
    private float initialYaw = 0f;

    private bool isFirstData = true;

    private bool isGameRunning = false;

    public float speed = 5000.0f;


    // Start is called before the first frame update
    void Start()
    {
        thread = new Thread(StartServer);
        thread.Start();
    }

    void StartServer()
    {
        listener = new TcpListener(IPAddress.Any, 25567);
        listener.Start();
        client = listener.AcceptTcpClient();
        stream = client.GetStream();

        while (true)
        {
            if (stream.DataAvailable)
            {
                int size = stream.Read(buffer, 0, buffer.Length);
                string data = Encoding.UTF8.GetString(buffer, 0, size);
                HandleData(data);
            }
        }
    }

    void HandleData(string data)
    {
        if(data.StartsWith("voice:")){
            string command = data.Substring(6);
            if (command == "1")
            {
                isGameRunning = true;
                Debug.Log("Game started");
            }
            else if (command == "0")
            {
                isGameRunning = false;
                Debug.Log("Game stopped");
            }

        }else if(isGameRunning){
            // Str to float
            string[] parts = data.Split(' ');
            pitch = -float.Parse(parts[0]);
            roll = float.Parse(parts[1]); 
            yaw = -float.Parse(parts[2]); 

            if (isFirstData)
            {
                initialPitch = pitch;
                initialRoll = roll;
                //initialYaw = (yaw + 180) % 360; // Reverse yaw by 180 degrees
                initialYaw = yaw;
                isFirstData = false;
            }
        }
        
    }

    // Update is called once per frame
    void Update()
    {
        float moveHorizontal = 0.0f;
        float moveVertical = 0.0f;

        if (isGameRunning)
        {
            
            Quaternion targetRotation = Quaternion.Euler(pitch - initialPitch, yaw - initialYaw, roll - initialRoll);
            float smoothFactor = 3.0f * Time.deltaTime; // Adjust this value as needed
            transform.rotation = Quaternion.Lerp(transform.rotation, targetRotation, smoothFactor);

            if (Input.GetKey(KeyCode.W))
                moveVertical = 2.0f;
            else if (Input.GetKey(KeyCode.S))
                moveVertical = -1.0f;
            else
                moveVertical = 1.0f;
            
            if (Input.GetKey(KeyCode.A))
                moveHorizontal = -1.0f;
            else if (Input.GetKey(KeyCode.D))
                moveHorizontal = 1.0f;
        

            Vector3 movement = new Vector3(moveHorizontal, 0.0f, moveVertical);
            movement = transform.TransformDirection(movement);

            transform.position = transform.position + movement * speed * Time.deltaTime;
        }else if (!isGameRunning){
            Vector3 movement = new Vector3(0.0f, 0.0f, 0.0f);
            movement = transform.TransformDirection(movement);

            transform.position = transform.position + movement * speed * Time.deltaTime;
        }
    }

    void OnApplicationQuit()
    {
        // close connection
        stream.Close();
        client.Close();
        listener.Stop();
        thread.Abort();
    }
}

package com.example.geeksmessanger;

import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.charset.Charset;

public class MainActivity extends AppCompatActivity {
    ConnectedThread mConnectedThread;
    String TAG = "TAG";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button Connect = (Button) findViewById(R.id.Connect);
        Connect.setOnClickListener(ButtonClickListener);
    }

    View.OnClickListener ButtonClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            switch (v.getId()) {
                case R.id.Connect:
                { 
                  ConnectThread ct = new ConnectThread();
                  ct.start();
                }
                break;
                default:
                    break;
            }
        }
    };

    public static byte[] intToByteArray(int a) {
        byte[] ret = new byte[4];
        ret[3] = (byte) (a & 0xFF);
        ret[2] = (byte) ((a >> 8) & 0xFF);
        ret[1] = (byte) ((a >> 16) & 0xFF);
        ret[0] = (byte) ((a >> 24) & 0xFF);
        return ret;
    }

    public static int ByteArrayToint (byte[] bytes) {
        int i = (bytes[0]<<24)&0xff000000 |
                (bytes[1]<<16)&0x00ff0000|
                (bytes[2]<<8)&0x0000ff00|
                (bytes[3]<<0)&0x000000ff;
        return i;
    }

    public void SendFile(String filename, int i) {
        try {
            try {
                Thread.sleep(3000);
            } catch (Exception e) {
                e.getLocalizedMessage();
            }
            FileInputStream fin = null;
            if (i == 1) {
                File file = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);
                File fin_0 = new File(file, filename);
                fin = new FileInputStream(fin_0);
            }
            if (i == 0) {
                fin = openFileInput(filename);
            }
            Log.d("read", "File size: bytes" + String.valueOf(fin.available()));
            int file_size = fin.available();
            if (file_size > 0) {
                byte[] buffer = new byte[file_size];
                fin.read(buffer, 0, file_size);
                String ss = String.valueOf(file_size);
                byte [] size = intToByteArray(file_size);
                Log.d("read", "file_size = " + ss);
                byte[] bytes = ss.getBytes(Charset.defaultCharset());
                mConnectedThread.write(size);
                try {
                    Thread.sleep(3000);
                } catch (Exception e) {
                    e.getLocalizedMessage();
                }
                mConnectedThread.write(buffer);
                try {
                    Thread.sleep(3000);
                } catch (Exception e) {
                    e.getLocalizedMessage();
                }
            } else {Log.d("read","No this file");}
        }
        catch(IOException ex){
            Log.d("read", "Sending file failed !");
        }

    }

    public void RecieveFile(String filename, int i) {
        try {
            try {
                Thread.sleep(3000);
            } catch (Exception e) {
                e.getLocalizedMessage();
            }
            FileOutputStream fin = null;
            if (i == 1) {
                File file = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);
                File fin_1 = new File(file, "media_robot");
                File fin_0 = new File(fin_1, filename);
                fin = new FileOutputStream(fin_0);
            }
            if (i == 0) {
                fin = openFileOutput(filename, Context.MODE_PRIVATE);
            }
            mConnectedThread.read(fin);
            try {
                Thread.sleep(3000);
            } catch (Exception e) {
                e.getLocalizedMessage();
            }
        }
        catch(Exception ex){
            Log.d("read", "Recieving file failed !");
        }
    }

private class ConnectThread extends Thread {
        private Socket mmSocket;

        public ConnectThread() {
            Log.d(TAG, "ConnectThread: started.");
        }

        public void run(){
            EditText IP = (EditText) findViewById(R.id.IP);
            EditText Port = (EditText) findViewById(R.id.Port);
            try {
                int a;
            } catch (Exception e) {
                try {
                    Log.d(TAG, "cancel: Closing Client Socket.");
                } catch (Exception e1) {
                    Log.e(TAG, "cancel: close() of mmSocket in Connectthread failed. " + e1.getMessage());
                }
                Log.d(TAG, "run: Closed Socket.");
                e.printStackTrace();
            }
            connected(mmSocket);
        }
        public void cancel() {
            try {
                Log.d(TAG, "cancel: Closing Client Socket.");
                mmSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "cancel: close() of mmSocket in Connectthread failed. " + e.getMessage());
            }
        }
    }

private void connected(Socket mmSocket) {
	Log.d(TAG, "connected: Starting.");
	try {
	    Log.d(TAG, "cancel: Closing Client Socket.");
	} catch (Exception e) {
	    Log.e(TAG, "cancel: close() of mmSocket in Connectthread failed. " + e.getMessage());
	}
	Intent i = new Intent(MainActivity.this, MainActivity2.class);
	EditText IP = (EditText) findViewById(R.id.IP);
	EditText Port = (EditText) findViewById(R.id.Port);
	i.putExtra("IP", IP.getText().toString());
	i.putExtra("Port", Port.getText().toString());
	startActivity(i);
}

private class ConnectedThread extends Thread {
    private final Socket mmSocket;
    private final InputStream mmInStream;
    private final OutputStream mmOutStream;

    public ConnectedThread(Socket socket) {
	Log.d(TAG, "ConnectedThread: Starting.");
	mmSocket = socket;
	InputStream tmpIn = null;
	OutputStream tmpOut = null;
	try {
	    tmpIn = mmSocket.getInputStream();
	    tmpOut = mmSocket.getOutputStream();
	} catch (IOException e) {
	    e.printStackTrace();
	}
	mmInStream = tmpIn;
	mmOutStream = tmpOut;
    }

    public void run(){
	byte[] buffer = new byte[1024];  // buffer store for the stream

	int bytes;
	while (true) {
	}
    }

    public void write(byte[] bytes) {
	String text = new String(bytes, Charset.defaultCharset());
	Log.d(TAG, "write: Writing to outputstream: " + text);
	try {
	    mmOutStream.write(bytes);
	} catch (IOException e) {
	    Log.e(TAG, "write: Error writing to output stream. " + e.getMessage() );
	}
    }

    public void read(FileOutputStream fos) {
	try {
	    byte[] bytes = new byte[4];
	    int len = mmInStream.read(bytes,0,4);
	    try {
	        Thread.sleep(3000);
	    } catch (Exception e) {
	        e.getLocalizedMessage();
	    }
	    Log.d("read", "Length_recieved = " + String.valueOf(len));
	    int length_file = ByteArrayToint (bytes);
	    Log.d("read","Length_file = " + String.valueOf(length_file));
	    int length_recive_iter;
	    byte[] bytes_f = new byte[length_file];
	    int a=0;
	    while (((length_recive_iter = mmInStream.read(bytes_f,0,length_file)) != a) || length_file == a) //-1
	    {
	        fos.write(bytes_f,0,length_recive_iter);
	        Log.d ("read", String.valueOf(length_recive_iter));
	        a=a+length_recive_iter;
	        Log.d ("read", "total = " + String.valueOf(a));
	        Log.d ("read", "length_of_recieved_bytes = " + String.valueOf(bytes_f.length));
	        if (length_file == a)
	        {
	            break;
	        }
	    }
	    fos.close();
	    try {
	        Thread.sleep(6000);
	    } catch (Exception e) {
	        e.getLocalizedMessage();
	    }
	} catch (IOException e) {
	    Log.e(TAG, "write: Error writing to output stream. " + e.getMessage() );
	}
    }

    public void cancel() {
	try {
	    mmSocket.close();
	} catch (IOException e) { }
    }
}

}

package com.example.geeksmessanger;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Toast;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.charset.Charset;


public class MainActivity2 extends AppCompatActivity {
    ConnectedThread mConnectedThread;
    String TAG = "TAG";
    String IP;
    Integer Port;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);
        ActivityCompat.requestPermissions(MainActivity2.this,
                new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.INTERNET}, 111);
        IP = getIntent().getStringExtra("IP");
        Port = Integer.valueOf(getIntent().getStringExtra("Port"));
        Button Send = (Button) findViewById(R.id.Send);
        Button GetImage = (Button) findViewById(R.id.GetImage);
        Send.setOnClickListener(ButtonClickListener);
        GetImage.setOnClickListener(ButtonClickListener);
        try {
               runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Button Send = (Button) findViewById(R.id.Send);
                    Send.setEnabled(true);
                }
            });
        } catch (Exception e) {
            Log.e(TAG, "write: Error reading Input Stream. " + e.getMessage() );
        }
    }


    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case 111: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // permission was granted, yay! Do the
                    // contacts-related task you need to do.
                } else {
                    // permission denied, boo! Disable the
                    // functionality that depends on this permission.
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(MainActivity2.this, "Permission denied to read your External storage", Toast.LENGTH_SHORT).show();
                        }
                    });
                }
                return;
            }

            // other 'case' lines to check for other
            // permissions this app might request
        }
    }

    View.OnClickListener ButtonClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            switch (v.getId()) {
                case R.id.Send:
                { 
                    try {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Button GetImage = (Button) findViewById(R.id.GetImage);
                                GetImage.setEnabled(false);
                                Button Send = (Button) findViewById(R.id.Send);
                                Send.setEnabled(false);
                            }
                        });
                        Thread thread = new Thread(new Runnable() {
                            @Override
                            public void run() {
                                try  {
                                    EditText Message = (EditText) findViewById(R.id.Messagew);
                                    String msg = Message.getText().toString();
                                    Log.d(TAG, "MSG. " + msg );
                                    byte[] msgBytes = msg.getBytes();
                                    byte[] EncreptedMsgBytes =  EncryptBytesVernamAlg(msgBytes);
                                    Socket socket = new Socket(IP,Port);
                                    DataOutputStream DOS = new DataOutputStream(socket.getOutputStream());
                                    DOS.write(EncreptedMsgBytes);
                                    DOS.flush();
                                    socket.close();
                                } catch (Exception e) {
                                    e.printStackTrace();
                                }
                            }
                        });
                        thread.start();
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                EditText Message = findViewById(R.id.Messagew);
                                Message.setText("");
                                Button GetImage = (Button) findViewById(R.id.GetImage);
                                GetImage.setEnabled(true);
                                Button Send = (Button) findViewById(R.id.Send);
                                Send.setEnabled(true);
                            }
                        });
                        try {
                            Thread.sleep(1000);
                        } catch (Exception e) {
                            e.getLocalizedMessage();
                        }
                        Button GetImgBtn = (Button)findViewById(R.id.GetImage);
                        GetImgBtn.performClick();
                    } catch (Exception e) {
                        Log.e(TAG, "write: Error reading Input Stream. " + e.getMessage() );
                    }
                }
                break;
                case R.id.GetImage:
                { 
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Button GetImage = (Button) findViewById(R.id.GetImage);
                            GetImage.setEnabled(false);
                            Button Send = (Button) findViewById(R.id.Send);
                            Send.setEnabled(false);
                        }
                    });
                    Thread thread = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            try  {
                                try {
                                    Thread.sleep(500);
                                } catch (Exception e) {
                                    e.getLocalizedMessage();
                                }
                                String msg = "IMG_GET";
                                String filename = "ScreenShot.jpg";
                                String filename_key = "Key.txt";
                                Socket socket = new Socket(IP,Port);
                                DataOutputStream DOS = new DataOutputStream(socket.getOutputStream());
                                Log.d(TAG, "MSG. " + msg );
                                byte[] msgBytes = msg.getBytes();
                                byte[] EncreptedMsgBytes =  EncryptBytesVernamAlg(msgBytes);
                                DOS.write(EncreptedMsgBytes);
                                DOS.flush();
                                FileOutputStream  fos = openFileOutput(filename, Context.MODE_PRIVATE);
                                Log.d(TAG, "Path = " + MainActivity2.this.getFilesDir().getAbsolutePath() );
                                    byte[] bytes = new byte[4];
                                    DataInputStream DIS = new DataInputStream(socket.getInputStream());
                                    int len = DIS.read(bytes,0,4);
                                    int length_file = ByteArrayToint (bytes);
                                    Log.d(TAG,"Length_file = " + String.valueOf(length_file));
                                    int length_recive_iter;
                                    byte[] bytes_f = new byte[length_file];
                                    int a=0;
                                    while (((length_recive_iter = DIS.read(bytes_f,0,length_file)) != 0) || length_file != a)
                                    {
                                        fos.write(bytes_f,0,length_recive_iter);
                                        Log.d (TAG, String.valueOf(length_recive_iter));
                                        a=a+length_recive_iter;
                                        Log.d (TAG, "total = " + String.valueOf(a));
                                        Log.d (TAG, "length_of_recieved_bytes = " + String.valueOf(length_recive_iter));
                                        if (length_file == a)
                                        {
                                            break;
                                        }
                                    }
                                    fos.close();
                                    DOS.close();
                                    DIS.close();
                                    Log.d(TAG, "File_received !");

                                FileInputStream fin = null;
                                fin = openFileInput(filename);
                                File file = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);
                                File file2 = new File(file, filename_key);
                                FileInputStream fin_key = null;
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                    }
                                    // ...
                                });
                                fin_key = new FileInputStream(file2);
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                    }
                                });
                                Log.d(TAG, "File size: bytes" + String.valueOf(fin.available()));
                                Log.d(TAG, "File_key size: bytes" + String.valueOf(fin_key.available()));
                                int file_size = fin.available();
                                int file_key_size = fin_key.available();
                                if (file_size > 0) {
                                    if (file_key_size < file_size)
                                    {
                                        Log.e(TAG, "file_key_size < file_size + offset_key !!!");
                                    }
                                    byte[] encrypted_data = new byte[file_size];
                                    fin.read(encrypted_data, 0, file_size);
                                    fin.close();
                                    byte[] key = new byte[file_key_size];
                                    fin_key.read(key, 0, file_key_size);
                                    fin_key.close();
                                    byte[] decrypted_data = new byte[file_size];
                                    for (int i=0; i < file_size; i++)
                                    {
                                        decrypted_data[i] = (byte) ((char)encrypted_data[i] ^ (char)key[i]);
                                    }
                                    FileOutputStream fos_decrypt = openFileOutput(filename, Context.MODE_PRIVATE);
                                    fos_decrypt.write(decrypted_data,0,file_size);
                                    fos_decrypt.close();
                                    FileOutputStream fos_key = new FileOutputStream(file2);
                                    fos_key.write(key,file_size,file_key_size - file_size);
                                    fos_key.close();
                                    Log.d(TAG,"Key_file_update");
                                    File fos_decrypt_N = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM);
                                    File fos_decrypt_N1 = new File(fos_decrypt_N, filename);
                                    FileOutputStream fosN = new FileOutputStream(fos_decrypt_N1);
                                    fosN.write(decrypted_data,0,file_size);
                                    fosN.close();
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            Toast.makeText(getApplicationContext(), "File_decrypted",
                                                    Toast.LENGTH_SHORT).show();
                                        }
                                    });

                                    Log.d(TAG,"File_decrypted");

                                } else {Log.d("read","No this file");}

                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            Bitmap myBitmap = BitmapFactory.decodeFile("/data/user/0/com.example.geeksmessanger/files/" + filename);
                                            ImageView myImage = (ImageView) findViewById(R.id.imageViewTest);
                                            myImage.setImageBitmap(myBitmap);
                                            try {
                                            } catch (Exception e) {
                                                e.getLocalizedMessage();
                                            }
                                            Button GetImage = (Button) findViewById(R.id.GetImage);
                                            GetImage.setEnabled(true);
                                            Button Send = (Button) findViewById(R.id.Send);
                                            Send.setEnabled(true);
                                        }
                                    });
                        }
                            catch (Exception e)
                            {
                                Log.d(TAG,"Error");
                            }
                        }
                    });

                    thread.start();
                }
                break;
                default:
                    break;
            }
        }
    };

    public byte[] EncryptBytesVernamAlg(byte[] bytes) {
        try {
        File file = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);
        String filename_key = "Key.txt";
        File file2 = new File(file, filename_key);
        FileInputStream fin_key = null;
        fin_key = new FileInputStream(file2);
        int file_key_size = fin_key.available();
        byte[] key = new byte[file_key_size];
        fin_key.read(key, 0, file_key_size);
        fin_key.close();
        int len_bytes = bytes.length;
        byte[] encrypted_data = new byte[len_bytes];
        for (int i = 0; i < len_bytes; i++) {
            encrypted_data[i] = (byte) ((char) bytes[i] ^ (char) key[i]);
        }
        FileOutputStream fos_key = new FileOutputStream(file2);
        fos_key.write(key, len_bytes, file_key_size - len_bytes);
        fos_key.close();
        Log.d("TAG", "Key_file_update");
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                //Toast.makeText(getApplicationContext(), "Data_encrypted",  Toast.LENGTH_LONG).show();
            }
        });
        Log.d("TAG", "Bytes_encrypted");
        return encrypted_data;
    } catch (Exception e)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), "Data_NOT_encrypted",   Toast.LENGTH_LONG).show();
            }
        });
        Log.d("TAG", "HREN_2");
        return bytes;
    }
   }

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

    public void SendMessage(View v) {
        EditText Message = findViewById(R.id.Messagew);
        byte[] bytes = Message.getText().toString().getBytes(Charset.defaultCharset());
        mConnectedThread.write(bytes);
    }

    public void SendCommand(String command) {
        String ss = command; // command may be
        try {
            mConnectedThread.write(ss.getBytes(Charset.defaultCharset()));
        }
        catch (Exception e)
        {}
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
                File fin_1 = new File(file, "GeeksMessanger");
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
        private Socket mmSocket = null;
        public ConnectThread() {
            Log.d(TAG, "ConnectThread: started.");
        }
        public void run(){
            try {
                Socket tmp = new Socket(IP, Port);
                mmSocket = tmp;
            } catch (IOException e) {
                try {
                    Log.d(TAG, "cancel: Closing Client Socket.");
                    mmSocket.close();
                } catch (IOException e1) {
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
        if (mmSocket != null)
        {
            mConnectedThread = new ConnectedThread(mmSocket);
            mConnectedThread.start();
            Button Send = (Button) findViewById(R.id.Send);
            try {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Button Send = (Button) findViewById(R.id.Send);
                        Button GetImage = (Button) findViewById(R.id.GetImage);
                        Send.setEnabled(true);
                        GetImage.setEnabled(true);
                    }
                });
            } catch (Exception e) {
                Log.e(TAG, "write: Error reading Input Stream. " + e.getMessage() );
            }
        }
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
            } catch (Exception e) {
                e.printStackTrace();
            }
            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        public void run(){
            byte[] buffer = new byte[1024]; 
            int bytes;
            while (true) {
            }
        }

        public void write(byte[] bytes) {
            String text = new String(bytes, Charset.defaultCharset());
            Log.d(TAG, "write: Writing to outputstream: " + text);
            try {
                DataOutputStream DOS = new DataOutputStream(mmOutStream);
                DOS. writeUTF("HELLO_WORLD");
                DOS.flush();
                DOS.close();
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
                        Log.d("read", "trying end!");
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

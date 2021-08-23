package com.example.getabed;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;

import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;

public class Splash_Screen extends AppCompatActivity {
    private FirebaseAuth auth;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash__screen);
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                auth=FirebaseAuth.getInstance();
                FirebaseUser user=auth.getCurrentUser();

                if(user!=null){
                    Intent intent = new Intent(Splash_Screen.this,Menu.class);
                    intent.putExtra("nombre",user.getDisplayName());
                    intent.putExtra("imagen",String.valueOf(user.getPhotoUrl()));
                    intent.putExtra("id",user.getUid());
                    startActivity(intent);
                    finish();

                }else{
                    Intent intent = new Intent(Splash_Screen.this,MainActivity.class);
                    startActivity(intent);
                    finish();

                }
           }
        },3000);


    }
}
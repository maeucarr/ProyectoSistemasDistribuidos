package com.example.getabed;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentTransaction;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.Toast;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.ArrayList;


public class EstadoCamas extends AppCompatActivity implements AdapterView.OnItemSelectedListener {
    Spinner spinner;
    LinearLayout view;
    CamasSeccion fragment;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_estado_camas);
        spinner= (Spinner) findViewById(R.id.spinner);
        inicializarSpinner();
        spinner.setOnItemSelectedListener(this);
        view= (LinearLayout) findViewById(R.id.view);
        Bundle bundle= new Bundle();
        bundle.putString("idSeccion","1");
        fragment= new CamasSeccion();
        fragment.setArguments(bundle);
        FragmentTransaction ft = getSupportFragmentManager().beginTransaction();

        ft.replace(R.id.your_placeholder, fragment);
        ft.commit();
    }

    //Inicializa Spinner con el nombre de las secciones
    private void inicializarSpinner(){
        FirebaseDatabase.getInstance().getReference()
                .child("hospital-prueba").child("secciones").addValueEventListener(
                        new ValueEventListener() {
                            @Override
                            public void onDataChange(@NonNull DataSnapshot snapshot) {
                                ArrayList<Seccion> arrayList= new ArrayList();
                                for(DataSnapshot child: snapshot.getChildren()){
                                    String nombre= (String) child.child("nombre").getValue();
                                    Seccion seccion= new Seccion(child.getKey(),nombre);
                                    arrayList.add(seccion);
                                }
                                int size= arrayList.size();
                                Seccion[] secciones= new Seccion[size];
                                for(int i =0 ; i<size;i++){
                                    Seccion sec= arrayList.get(i);
                                    secciones[i]= sec;
                                }
                                SpinAdapter arrayAdapter= new SpinAdapter(getApplicationContext(), android.R.layout.simple_spinner_item,secciones);

                                spinner.setAdapter(arrayAdapter);
                            }

                            @Override
                            public void onCancelled(@NonNull DatabaseError error) {

                            }
                        }
                );

    }

    public void irGrafico(View view){
        Intent intent= new Intent(getBaseContext(),GraficoDisponibilidad.class);
        Seccion seccion= (Seccion) spinner.getSelectedItem();
        intent.putExtra("seccionId",seccion.getId());
        startActivity(intent);
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            Seccion seccion = (Seccion) spinner.getItemAtPosition(position);
            fragment.obtenerCamas(seccion.getId());
        Log.d("Seleccion", "onItemSelected: Seccion");
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {
    }
}
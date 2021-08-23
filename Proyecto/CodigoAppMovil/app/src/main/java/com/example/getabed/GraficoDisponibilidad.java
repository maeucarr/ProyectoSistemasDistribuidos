package com.example.getabed;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import com.github.mikephil.charting.charts.PieChart;
import com.github.mikephil.charting.data.PieData;
import com.github.mikephil.charting.data.PieDataSet;
import com.github.mikephil.charting.data.PieEntry;
import com.github.mikephil.charting.utils.ColorTemplate;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.ArrayList;
import java.util.List;

public class GraficoDisponibilidad extends AppCompatActivity {
    String idSeccion;
    PieChart pieChart;
    PieData pieData;
    PieDataSet pieDataSet;
    List<PieEntry> pieEntries;
    ArrayList PieEntryLabels;
    TextView nombreView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_grafico_disponibilidad);
        Intent intent= getIntent();
        idSeccion= intent.getStringExtra("seccionId");
        Log.d("cama", "onCreate: "+idSeccion);
        pieChart = findViewById(R.id.pieChart);
        nombreView= findViewById(R.id.nombreView);
        obtenerNombre();
        obtenerInfo();
    }

    //obtenerInfo:
    //Metodo que obtiene la informacion de las camas de firebase y construye el grafico
    private void  obtenerInfo(){
        FirebaseDatabase.getInstance().getReference().child("hospital-prueba")
                .child("secciones").child(idSeccion).child("camas").addValueEventListener(
                new ValueEventListener() {
                    @Override
                    public void onDataChange(@NonNull DataSnapshot snapshot) {
                        int disponibles=0;
                        int ocupadas=0;
                        for(DataSnapshot child: snapshot.getChildren()){
                            String estado= (String) child.child("dispositivo").child("estado").getValue();
                            if(estado.equals("Ocupado")){
                                ocupadas++;
                            }else{
                                disponibles++;
                            }
                        }
                        Log.d("cama", "onDataChange: "+Integer.toString(ocupadas));
                        pieEntries= new ArrayList();
                        pieEntries.add(new PieEntry(ocupadas,"Ocupadas"));
                        pieEntries.add(new PieEntry(disponibles,"Disponibles"));
                        pieDataSet = new PieDataSet(pieEntries, "Disponibilidad de camas");
                        pieData = new PieData(pieDataSet);
                        pieChart.setData(pieData);
                        pieDataSet.setColors(Color.RED, Color.rgb(23,169,61));
                        pieDataSet.setSliceSpace(2f);
                        pieDataSet.setValueTextColor(Color.WHITE);
                        pieDataSet.setValueTextSize(20f);
                        pieDataSet.setSliceSpace(5f);
                        pieChart.invalidate();
                    }

                    @Override
                    public void onCancelled(@NonNull DatabaseError error) {

                    }
                }
        );
    }

    private  void obtenerNombre(){
        FirebaseDatabase.getInstance().getReference().child("hospital-prueba")
                .child("secciones").child(idSeccion).child("nombre").addValueEventListener(
                new ValueEventListener() {
                    @Override
                    public void onDataChange(@NonNull DataSnapshot snapshot) {
                        nombreView.setText((String) snapshot.getValue());
                    }

                    @Override
                    public void onCancelled(@NonNull DatabaseError error) {

                    }
                }
        );
    }

}
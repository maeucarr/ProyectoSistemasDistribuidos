package com.example.getabed;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.GridView;
import android.widget.TextView;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.ArrayList;
import java.util.List;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link CamasSeccion#newInstance} factory method to
 * create an instance of this fragment.
 */
public class CamasSeccion extends Fragment {
    GridView gridView;

    public CamasSeccion() {
        // Required empty public constructor
    }

   // TODO: Rename and change types and number of parameters
    public static CamasSeccion newInstance(String param1, String param2) {
        CamasSeccion fragment = new CamasSeccion();
        Bundle args = new Bundle();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {


        return inflater.inflate(R.layout.fragment_camas, container, false);
    }

    @Override
    public void onViewCreated(View view , Bundle savedInstanceState){
        super.onViewCreated(view,savedInstanceState);
        gridView= (GridView) view.findViewById(R.id.gridview);
        obtenerCamas("1");

    }

    public void obtenerCamas(String id){
        FirebaseDatabase.getInstance().getReference().child("hospital-prueba/secciones/").child(id).child("camas").addValueEventListener(
                new ValueEventListener() {
                    @Override
                    public void onDataChange(@NonNull DataSnapshot snapshot) {
                        ArrayList<Cama> camas= new ArrayList<Cama>();
                        for(DataSnapshot child: snapshot.getChildren()){
                            String estado= (String) child.child("dispositivo").child("estado").getValue();
                            String id= (String) child.getKey();
                            String piso= (String) child.child("piso").getValue().toString();
                            String bateria = (String) child.child("dispositivo").child("bateria").getValue().toString();
                            Cama cama= new Cama(id,estado,piso,bateria);
                            camas.add(cama);
                        }
                        CamasAdapter adapter= new CamasAdapter(getContext(),camas);
                        gridView.setAdapter(adapter);
                        gridView.setGravity(Gravity.CENTER);
                    }

                    @Override
                    public void onCancelled(@NonNull DatabaseError error) {

                    }
                }
        );
    }
}
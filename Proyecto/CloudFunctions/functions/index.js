const functions = require("firebase-functions");
const admin = require("firebase-admin");
admin.initializeApp();

exports.enviarNotificaciones = functions.database
  .ref(
    "/hospital-prueba/secciones/{seccionId}/camas/{camaId}/dispositivo/estado"
  )
  .onUpdate(async (snap, context) => {
    const seccionId = context.params.seccionId;
    const camaId = context.params.camaId;
    const estado = snap.after.val();
    if (estado === "Desocupado") {
      notificarPendientes();
    }
  });

notificarPendientes = async () => {
  const enfermerosSnapshot = await admin
    .database()
    .ref("/hospital-prueba/enfermeros")
    .once("value");
  const enfermeros = Object.values(enfermerosSnapshot.val());
  let tokens = [];
  for (let enfermero of enfermeros) {
    if (enfermero.estado == "En espera") {
      tokens.push(enfermero.token_notification);
    }
  }
  const payload = {
    notification: {
      title: "GetABed ",
      body: `Una cama ha sido desocupada`,
    },
  };
  await admin.messaging().sendToDevice(tokens, payload);
};

cambiarEstadoEnfermeros = async () => {};

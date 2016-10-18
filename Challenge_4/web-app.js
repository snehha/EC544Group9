<script src="https://www.gstatic.com/firebasejs/3.5.0/firebase.js"></script>
<script>
  // Initialize Firebase
  var config = {
    apiKey: "AIzaSyBb-XwHn8JTniCY8Y0G9MhrOMJhPEF7xdI",
    authDomain: "group9-node.firebaseapp.com",
    databaseURL: "https://group9-node.firebaseio.com",
    storageBucket: "group9-node.appspot.com",
    messagingSenderId: "985259766468"
  };
  firebase.initializeApp(config);
</script>

// Sets up shortcuts to Firebase features and initiate firebase auth.
FriendlyChat.prototype.initFirebase = function() {
  // Shortcuts to Firebase SDK features.
  this.auth = firebase.auth();
  this.database = firebase.database();
  this.storage = firebase.storage();
  // Initiates Firebase auth and listen to auth state changes.
  this.auth.onAuthStateChanged(this.onAuthStateChanged.bind(this));
};
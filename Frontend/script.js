document.getElementById("fingerprintBtn").addEventListener("click", () => {
    document.getElementById("voterSection").classList.remove("hidden");
  });
  
  document.getElementById("verifyBtn").addEventListener("click", async () => {
    const voterId = document.getElementById("voterIdInput").value.trim();
    const resultDiv = document.getElementById("resultMessage");
  
    if (!voterId || isNaN(voterId)) {
        resultDiv.textContent = "Please enter a valid Voter ID.";
        return;
      }
  
    try {
      resultDiv.textContent = "Verifying fingerprint...";
      
      const response = await fetch("http://192.168.121.56:3000/vote", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({ voterId: parseInt(voterId) }),
      });
  
      const data = await response.json();
      resultDiv.textContent = data.message || "Response received.";
      
      if (data.status === "fallback") {
        document.getElementById("faceBtn").disabled = false;
        document.getElementById("voiceBtn").disabled = false;
      }
      
  
    } catch (err) {
      resultDiv.textContent = "Error connecting to server.";
      console.error(err);
    }
  });
  
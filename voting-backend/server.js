const express = require("express");
const Web3 = require("web3").default;
const cors = require("cors");
require("dotenv").config();


const app = express();
app.use(cors());
app.use(express.json());

const web3 = new Web3("http://localhost:9545"); // Ganache
const contractABI = [
  {
    "inputs": [],
    "stateMutability": "nonpayable",
    "type": "constructor"
  },
  {
    "anonymous": false,
    "inputs": [
      {
        "indexed": false,
        "internalType": "uint256",
        "name": "voterId",
        "type": "uint256"
      }
    ],
    "name": "VoteCast",
    "type": "event"
  },
  {
    "inputs": [],
    "name": "admin",
    "outputs": [
      {
        "internalType": "address",
        "name": "",
        "type": "address"
      }
    ],
    "stateMutability": "view",
    "type": "function"
  },
  {
    "inputs": [
      {
        "internalType": "uint256",
        "name": "",
        "type": "uint256"
      }
    ],
    "name": "hasVoted",
    "outputs": [
      {
        "internalType": "bool",
        "name": "",
        "type": "bool"
      }
    ],
    "stateMutability": "view",
    "type": "function"
  },
  {
    "inputs": [
      {
        "internalType": "uint256",
        "name": "voterId",
        "type": "uint256"
      }
    ],
    "name": "vote",
    "outputs": [],
    "stateMutability": "nonpayable",
    "type": "function"
  },
  {
    "inputs": [
      {
        "internalType": "uint256",
        "name": "voterId",
        "type": "uint256"
      }
    ],
    "name": "checkVoted",
    "outputs": [
      {
        "internalType": "bool",
        "name": "",
        "type": "bool"
      }
    ],
    "stateMutability": "view",
    "type": "function"
  }
];
const contractAddress = "0x60f206D56F71214ecACe2af5b83AbE440116454e"; // Deployed contract address

const contract = new web3.eth.Contract(contractABI, contractAddress);
const senderAddress = "0x51BBF04739Af2d3F516323B46E9E32e2b22b56FA"; // First account from Ganache

app.post("/vote", async (req, res) => {
  const { voterId } = req.body;
  try {
    await contract.methods.vote(voterId).send({ from: senderAddress });
    res.status(200).send("Vote cast successfully!");
  } catch (err) {
    res.status(400).send(err.message);
  }
});
app.get('/', (req, res) => {
  res.send('Biometric Voting Backend is up and running!');
});


app.listen(3000, () => {
  console.log(" Server running on port http://localhost:3000");
});

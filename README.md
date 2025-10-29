# Crypto Rootkit

A 3-minute video demonstrating usage of the rootkit can be found [**here**](https://www.youtube.com/watch?v=UyfCoS1v6m0).  
A Linux kernel rootkit designed to hide a simulated crypto-mining program.  
This project was created for **educational and research purposes only**.

---

## 📄 Project Specification
You can read the full project specification and deliverables here:  
[**Project Rootkit – Deliverables (PDF)**](https://github.com/yousuf865/Project-Rootkit/blob/main/Project%20Rootkit%20Deliverables.pdf)

---

## ⚠️ Disclaimer
This software is provided **for educational and research purposes only**.  
The author:
- Does **not** condone or promote the use of this code for malicious purposes.
- Shall **not** be held liable for any damage, loss of data, legal consequences, or misuse of this software.
- Assumes that anyone using or modifying this code is doing so in a safe, legal, and controlled environment (e.g., isolated virtual machines).

By using or referencing this repository, you agree that you take **full responsibility** for your actions.

---

## 🛠 Features
- Hides the rootkit module from `/proc/modules`.
- Runs a simulated crypto-mining daemon (no real cryptocurrency mining).
- Hides the running crypto-mining process by name.

---

## 📚 Requirements
- Linux (specific Ubuntu version specified in Deliverables)
- `make` and `gcc`

---

## 🚀 Usage
```bash
# Build the kernel module
make

# Load the module
sudo insmod rootkit.ko

# Remove the module
sudo reboot

# NOTE:
sudo rmmod rootkit.ko will NOT work. An entire reboot of the system is required to remove the rootkit. This is because the rootkit kernel module is hidden, so as far as the system is concerned, rootkit.ko does not even exist.

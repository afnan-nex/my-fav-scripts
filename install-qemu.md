# Complete Guide to QEMU & Virtual Machine Setup
### Universal Guide for Linux, Windows, and macOS

---

## Table of Contents
1. [What is QEMU & Native Accelerators](#1-what-is-qemu--native-accelerators)
2. [Critical Precautions Before You Begin](#2-critical-precautions-before-you-begin)
3. [Platform Installation Guides](#3-platform-installation-guides)
   - [Linux (Arch, Ubuntu/Debian, Fedora)](#a-linux-best-performance-via-kvm)
   - [Windows (10 / 11 via WHPX)](#b-windows-10--11-using-whpx)
   - [macOS (Apple Silicon & Intel via UTM)](#c-macos-apple-silicon-m-series--intel)
4. [How to Create and Run Virtual Machines](#4-how-to-create-and-run-virtual-machines)
   - [Method 1: GUI (Virt-Manager)](#method-1-using-virt-manager-gui-recommended-on-linux)
   - [Method 2: Command-Line (CLI)](#method-2-command-line-cli-with-virt-install)
5. [Enabling Shared Clipboard & File Transfers](#5-enabling-shared-clipboard--file-transfers)
   - [Clipboard & Auto-Resize (SPICE Guest Tools)](#1-clipboard--auto-resize-spice-guest-tools)
   - [Sharing Full Folders & Directories (Samba Drive Z:\)](#2-sharing-full-folders--directories-samba-mapped-drive)
6. [How to Change VM Configurations](#6-how-to-change-vm-configurations)
   - [Adjusting RAM & CPU Cores](#1-adjusting-ram--cpu-cores)
   - [Expanding Disk Space](#2-expanding-disk-space)
   - [Changing Boot Order & CD-ROMs](#3-changing-boot-order--ejecting-isos)
7. [Troubleshooting & Gotchas](#7-troubleshooting--common-gotchas)

---

## 1. What is QEMU & Native Accelerators

**QEMU** is a fast, open-source machine emulator and virtualizer. 

When paired with your operating system's native hardware hypervisor, it runs virtual machines at **near-native bare-metal speed**:
* **Linux:** Uses **KVM** (Kernel-based Virtual Machine) — *Fastest*
* **Windows:** Uses **WHPX** (Windows Hypervisor Platform)
* **macOS:** Uses **HVF** (Hypervisor.framework)

---

## 2. Critical Precautions Before You Begin

1. **Enable Hardware Virtualization in BIOS/UEFI:**
   * **Intel CPUs:** Ensure **Intel Virtualization Technology (VT-x)** is set to **Enabled**.
   * **AMD CPUs:** Ensure **SVM Mode (Secure Virtual Machine)** or **AMD-V** is set to **Enabled**.
2. **Always Use SSD Storage (Avoid HDDs):**
   * Storing virtual machine disks on a mechanical HDD causes high disk latency, stuttering, and slow boots. Always use an internal NVMe/SATA SSD or fast external SSD.
3. **Use `qcow2` Dynamic Disks:**
   * Always choose the `qcow2` format over raw images. A 50 GB `qcow2` disk only takes ~200 KB initially and dynamically grows only as you write data inside the guest OS.
4. **Storage Bus for Windows Guests:**
   * Use **SATA bus** for the virtual disk during initial Windows install so Windows natively detects the drive without needing third-party drivers.

---

## 3. Platform Installation Guides

### A. Linux (Best Performance via KVM)

Linux offers the lowest latency and highest performance using KVM with **Virt-Manager** as the desktop application.

#### Step 1: Install Packages

**Arch Linux / Omarchy / Manjaro:**
```bash
sudo pacman -S --needed --noconfirm qemu-desktop virt-manager libvirt dnsmasq edk2-ovmf swtpm virt-viewer
```

**Ubuntu / Debian / Linux Mint:**
```bash
sudo apt update && sudo apt install -y qemu-system-x86 qemu-utils virt-manager libvirt-daemon-system ovmf swtpm virt-viewer dnsmasq
```

**Fedora / RHEL:**
```bash
sudo dnf install -y @virtualization virt-manager virt-viewer edk2-ovmf swtpm
```

#### Step 2: Enable Virtualization Service & Permissions
```bash
sudo usermod -aG libvirt,kvm $USER && sudo systemctl enable --now libvirtd
```

#### Step 3: Fix Python Version Conflicts (If using mise, pyenv, conda, or asdf)
If running `virt-manager` throws `ModuleNotFoundError: No module named 'gi'`, redirect it to system Python:
```bash
cat << 'EOF' > ~/.local/bin/virt-manager
#!/bin/sh
exec /usr/bin/python3 /usr/bin/virt-manager "$@"
EOF
chmod +x ~/.local/bin/virt-manager
```

---

### B. Windows 10 / 11 (Using WHPX)

On Windows hosts, QEMU uses the Windows Hypervisor Platform (WHPX) accelerator.

#### Step 1: Enable Hypervisor Platform
Open **PowerShell as Administrator** and run:
```powershell
Enable-WindowsOptionalFeature -Online -FeatureName HypervisorPlatform -All
```
*(Restart your computer if prompted).*

#### Step 2: Install QEMU

**Via Winget:**
```powershell
winget install SoftwareFreedomConservancy.QEMU
```

**Via Chocolatey:**
```powershell
choco install qemu
```

**Recommended GUI Frontends for Windows:**
* **Quickemu / Quickgui:** The most user-friendly desktop GUI wrapper for QEMU on Windows.
* **WSL2 + Virt-Manager:** Run Linux Virt-Manager directly on Windows via WSLg.

---

### C. macOS (Apple Silicon M-Series & Intel)

On macOS, the premier native application powered by QEMU is **UTM**.

#### Step 1: Install UTM (Recommended Native GUI)
```bash
brew install --cask utm
```
*(Or download the installer directly from [mac.getutm.app](https://mac.getutm.app/)).*

UTM gives macOS users full retina display scaling, dynamic clipboard sharing, and folder sharing out of the box.

#### Step 2: Install QEMU CLI (Optional for terminal power users)
```bash
brew install qemu
```

---

## 4. How to Create and Run Virtual Machines

### Method 1: Using Virt-Manager GUI (Recommended on Linux)

1. Launch **Virtual Machine Manager** from your application menu or run:
```bash
virt-manager
```
2. Click the **New Virtual Machine** button (top-left).
3. Choose **Local install media (ISO image)** and click **Forward**.
4. Click **Browse...** and select your OS `.iso` file.
5. Allocate Memory and CPU:
   * **RAM:** Set at least **4096 MB** (4 GB).
   * **CPUs:** Set at least **2 cores**.
6. Select your disk location on your fast SSD partition and specify the size (e.g. 40 GB).
7. Check **Customize configuration before install** and click **Finish**:
   * Set **Disk 1** bus to **SATA**.
   * Set **Display** to **Spice**.
   * Set **Video** to **QXL**.
8. Click **Begin Installation**.

---

### Method 2: Command-Line (CLI) with `virt-install`

#### Step 1: Create a 40 GB dynamic disk image on your SSD:
```bash
qemu-img create -f qcow2 "/path/to/vm_storage/windows10.qcow2" 40G
```

#### Step 2: Launch the VM with optimized settings:
```bash
virt-install --connect qemu:///session \
  --name win10-vm \
  --os-variant win10 \
  --memory 4096 \
  --vcpus 2 \
  --disk path="/path/to/vm_storage/windows10.qcow2",bus=sata \
  --cdrom "/path/to/windows.iso" \
  --graphics spice \
  --video qxl \
  --network user,model=e1000e \
  --noautoconsole
```

#### Step 3: Open the graphical display window:
```bash
virt-viewer --connect qemu:///session win10-vm
```

---

## 5. Enabling Shared Clipboard & File Transfers

### 1. Clipboard & Auto-Resize (SPICE Guest Tools)

To enable **copy/paste between host and guest** and **auto-resizing resolution**:

1. Download the guest tools installer on the host:
```bash
curl -L -o spice-guest-tools-latest.exe https://www.spice-space.org/download/windows/spice-guest-tools/spice-guest-tools-latest.exe
```
2. Transfer `spice-guest-tools-latest.exe` into Windows (via CD image, USB pass-through, or browser).
3. Double-click to run the installer inside Windows and reboot the VM.
4. **Done!** Bidirectional clipboard and smooth mouse are active.

---

### 2. Sharing Full Folders & Directories (Samba Mapped Drive)

SPICE drag-and-drop only works for individual files. To copy and access **full folders, project trees, and subdirectories**, map a network drive.

#### Setup on the Linux Host:
1. Install Samba:
```bash
sudo pacman -S --needed --noconfirm samba
```
*(On Ubuntu: `sudo apt install -y samba`)*

2. Create a shared directory:
```bash
mkdir -p ~/VM_Shared
```

3. Add the share to `/etc/samba/smb.conf`:
```ini
[global]
   workgroup = WORKGROUP
   server string = Linux VM Share
   server role = standalone server
   map to guest = Bad User
   log file = /var/log/samba/%m.log
   max log size = 50

[VM_Shared]
   comment = Shared Directory for VM
   path = /home/mas/VM_Shared
   read only = no
   browsable = yes
   guest ok = yes
   writable = yes
   create mask = 0777
   directory mask = 0777
   force user = mas
```

4. Set a Samba password and start the service:
```bash
(echo "1234"; echo "1234") | sudo smbpasswd -a -s $USER && sudo systemctl enable --now smb
```

#### Connect Inside the Windows Guest:
1. Press `Win + E` to open **File Explorer**.
2. Right-click **This PC** -> **Map network drive...**
3. Select Drive `Z:` and enter:
```text
\\10.0.2.2\VM_Shared
```
4. Click **Finish**. (If prompted for credentials, use your Linux username and the password configured above).
5. You can now drag, drop, and edit entire directories in both directions at full SSD speed!

---

## 6. How to Change VM Configurations

Always shut down the VM before modifying core hardware settings.

### Using Virt-Manager GUI:
1. Open `virt-manager`.
2. Double-click the VM, then click the **Lightbulb icon** ("Show virtual hardware details").
3. Select **CPUs** or **Memory** to change core counts or RAM amounts.
4. Click **Apply** in the lower-right corner.

### Using the Command Line:

#### 1. Adjusting RAM & CPU Cores
```bash
# Change memory to 6 GB
virsh --connect qemu:///session setmaxmem win10-vm 6G --config
virsh --connect qemu:///session setmem win10-vm 6G --config

# Change CPU cores to 4
virsh --connect qemu:///session setvcpus win10-vm 4 --config --maximum
virsh --connect qemu:///session setvcpus win10-vm 4 --config
```

#### 2. Expanding Disk Space
To add 20 GB to an existing virtual disk:
```bash
qemu-img resize "/path/to/vm_storage/windows10.qcow2" +20G
```
*After resizing, boot Windows, open **Disk Management** (`diskmgmt.msc`), right-click your `C:` partition, and click **Extend Volume**.*

#### 3. Starting and Stopping VMs via CLI
```bash
# Start VM
virsh --connect qemu:///session start win10-vm

# Gracefully shut down VM
virsh --connect qemu:///session shutdown win10-vm

# Force power off VM
virsh --connect qemu:///session destroy win10-vm
```

---

## 7. Troubleshooting & Common Gotchas

| Issue | Cause | Fix |
| :--- | :--- | :--- |
| **`Could not access KVM kernel module: /dev/kvm`** | Hardware virtualization disabled in BIOS or missing user group permissions | Enable VT-x / AMD-V in BIOS. Run `sudo usermod -aG kvm $USER` and reboot or re-login. |
| **Windows installer shows "No drives were found"** | Windows installer lacks VirtIO storage drivers | Set the virtual disk bus to `sata` instead of `virtio`. |
| **`ModuleNotFoundError: No module named 'gi'`** | Python version manager (mise/pyenv/conda) shadowing system Python | Create wrapper in `~/.local/bin/virt-manager` pointing to `/usr/bin/python3`. |
| **Resolution stuck at 800x600 & sluggish mouse** | Missing SPICE display drivers | Install `spice-guest-tools-latest.exe` inside Windows and reboot. |
| **Cannot drag entire folders/directories into VM** | SPICE drag-and-drop only supports individual files | Use the Samba Mapped Drive (`\\10.0.2.2\VM_Shared`) or zip the folder before dragging. |
| **VM is extremely slow and freezing** | VM disk placed on mechanical HDD or only 1 CPU allocated | Relocate virtual disk to an SSD partition and allocate at least 2 vCPUs. |

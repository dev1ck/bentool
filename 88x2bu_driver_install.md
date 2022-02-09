# Step 1: Open a terminal (e.g. Ctrl+Alt+T)

# Step 2: Update the system package information (select the option for the OS you are using)
 - sudo apt update

# Step 3: Install the required packages (select the option for the OS you are using)
 - sudo apt install -y linux-headers-$(uname -r) build-essential dkms git libelf-dev

# Step 4: Create a directory to hold the downloaded driver
 - mkdir -p ~/src

# Step 5: Move to the newly created directory
 - cd ~/src

# Step 6: Download the driver
 - git clone https://github.com/morrownr/88x2bu-20210702.git

# Step 7: Move to the newly created driver directory
 - cd ~/src/88x2bu-20210702

# Step 8: Run the installation script ( install-driver.sh )
 - sudo ./install-driver.sh

OR

- make clean
- make
- sudo make install
- sudo reboot
# **F1Visualizer**

Visualizes Formula 1 race data using Min-Heap and B+ Tree approaches. The app is powered by a Dash frontend and a C++ backend.

*Unfortunately our program only works on windows*

---

## **Features**
- Displays the top 5 drivers for a selected race and lap.
- Includes two approaches for processing race data:
  - **Min-Heap Approach**
  - **B+ Tree Approach**
- Real-time updates based on the data set for race data when navigating laps.

---

## **Run Instructions**

### 1. **Clone the Repository**
```bash
git clone https://github.com/abebanos/F1Visualizer.git
cd F1Visualizer
```

### 2. **Set Up the Python Environment**

If Python 3.13 is not installed, download and install it from the official Python website or use the following commands based on your operating system:

Windows:

Open Command Prompt as Administrator.

Run the following command to update Python:

```bash
python -m ensurepip --upgrade
python -m pip install --upgrade python==3.13
```

If not installed, download the installer from the Python website.

Run the installer and ensure you check the option to add Python to PATH.

#### Create a virtual environment:
```bash
python -m venv venv
```

#### Activate the virtual environment:

**Windows:**
```bash
venv\Scripts\activate
```

**Linux/Mac:**
```bash
source venv/bin/activate
```

#### Install the required Python dependencies:
```bash
pip install -r requirements.txt
```

#### Ensure the executable `F1Visualizer.exe` is located in the `cmake-build-debug` directory.


### 3. **Running the Application**

#### Start the Python Dash app:
```bash
python dash_app.py
```

#### Open your browser and navigate to the provided link:
```
http://127.0.0.1:8050/
This link may be different for you
```

### **Usage**

#### Start Race
1. Enter the Race ID in the input field.
2. Click the **Start Race** button to display the results for the first lap.

#### Navigate Laps
- Use the **Next Lap** and **Previous Lap** buttons to switch between laps.

### **Output**
The top 5 drivers for the selected race and lap are displayed for:
- Min-Heap Approach.
- B+ Tree Approach.


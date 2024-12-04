import dash
import dash_bootstrap_components as dbc
from dash import dcc, html, Input, Output, State
import json
import subprocess
import os


app = dash.Dash(__name__, external_stylesheets=[dbc.themes.CYBORG])
app.title = "F1Visualizer - Top 5 Drivers"

# Load C++ JSON
def load_results():
    try:
        with open("cmake-build-debug/results.json", "r") as file:
            return json.load(file)
    except FileNotFoundError:
        return []

current_lap = 1

def run_cpp_program(race_id, lap_number):
    try:
        executable_path = os.path.abspath("cmake-build-debug/F1Visualizer.exe")
        working_directory = os.path.abspath("cmake-build-debug")

        # Debugging
        print(f"Attempting to run: {executable_path} {race_id} {lap_number}")
        print(f"Working directory: {working_directory}")

        # Run C++ program
        result = subprocess.run(
            [executable_path, str(race_id), str(lap_number)],
            cwd=working_directory,
            env=os.environ,
            capture_output=True,
            text=True,
            check=True
        )

        runtime = None
        for line in result.stdout.splitlines():
            if "Min-Heap Build & Query Runtime:" in line:
                runtime = line.split(":")[1].strip()

        # More debugging
        print("Program output:", result.stdout)
        print("Program errors (if any):", result.stderr)

        return runtime

    except subprocess.CalledProcessError as e:
        print(f"Error running C++ program: {e}")
        print(f"Return code: {e.returncode}")
        print("stdout: {e.stdout}")
        print("stderr: {e.stderr}")
        return None
    except FileNotFoundError:
        print("C++ executable not found! Ensure the correct path is set.")
        return None

# Frontend
app.layout = dbc.Container(
    fluid=True,
    children=[
        html.Div(
            [
                html.H1(
                    "Formula 1 Visualizer",
                    className="text-center",
                    style={"font-size": "3em", "color": "#FFFFFF"},
                ),
                html.H2(
                    "Race",
                    className="text-center",
                    style={"font-size": "2em", "color": "#A020F0"},
                ),
                html.H3(
                    id="lap-info",
                    className="text-center",
                    style={"font-size": "1.5em", "margin-top": "20px"},
                ),
            ],
            style={"background-color": "#222222", "padding": "20px", "border-radius": "15px"},
        ),

        dbc.Row(
            [
                dbc.Col(
                    dbc.Input(id="race-id", type="number", placeholder="Enter Race ID", className="mb-3"),
                    width=6,
                ),
            ],
            justify="center",
            style={"margin-top": "20px"},
        ),
        dbc.Button(
            "Start Race",
            id="start-btn",
            color="primary",
            className="d-block mx-auto",
            style={"margin-top": "20px"},
        ),

        # Buttons
        html.Div(
            [
                dbc.Button("Previous Lap", id="prev-lap-btn", n_clicks=0, color="secondary", style={"margin-right": "10px"}),
                dbc.Button("Next Lap", id="next-lap-btn", n_clicks=0, color="success"),
            ],
            className="text-center",
            style={"margin-top": "20px"},
        ),

        html.Div(
            [
                html.H3(
                    "Min-Heap Approach",
                    className="text-center",
                    style={"color": "#FFD700", "margin-bottom": "20px"},
                ),
                dbc.Table(
                    id="min-heap-table",
                    striped=True,
                    bordered=True,
                    hover=True,
                    style={"background-color": "#444444", "color": "white"},
                ),
                html.Div(
                    id="min-heap-time",
                    className="text-center",
                    style={"color": "#A9A9A9", "margin-top": "10px"},
                ),

                html.H3(
                    "B+ Tree Approach",
                    className="text-center",
                    style={"color": "#FFD700", "margin-bottom": "20px"},
                ),
                dbc.Table(
                    id="bplus-tree-table",
                    striped=True,
                    bordered=True,
                    hover=True,
                    style={"background-color": "#444444", "color": "white"},
                ),
                html.Div(
                    id="bplus-tree-time",
                    className="text-center",
                    style={"color": "#A9A9A9", "margin-top": "10px"},
                ),
            ],
            style={"margin-top": "20px"},
        ),
    ],
    style={"padding": "20px"},
)

@app.callback(
    [
        Output("lap-info", "children"),
        Output("min-heap-table", "children"),
        Output("min-heap-time", "children"),
        Output("bplus-tree-table", "children"),
        Output("bplus-tree-time", "children"),
    ],
    [Input("start-btn", "n_clicks"), Input("next-lap-btn", "n_clicks"), Input("prev-lap-btn", "n_clicks")],
    [State("race-id", "value")]
)
def update_dashboard(start_clicks, next_clicks, prev_clicks, race_id):
    global current_lap

    if not start_clicks and not next_clicks and not prev_clicks:
        return "Enter Race ID to start the simulation.", [], "", [], ""

    if race_id is None:
        return "Please enter a valid Race ID.", [], "", [], ""

    triggered = dash.callback_context.triggered[0]["prop_id"]
    if "start-btn" in triggered:
        current_lap = 1
    elif "next-lap-btn" in triggered:
        current_lap += 1
    elif "prev-lap-btn" in triggered and current_lap > 1:
        current_lap -= 1

    # Run program and update JSON
    run_cpp_program(race_id, current_lap)

    # Load results.json
    results = load_results()
    if not results:
        return f"No results available for Race {race_id}, Lap {current_lap}.", [], "Runtime: N/A", [], "Runtime: N/A"

    # Extract results
    min_heap_results = results.get("minHeapResults", [])
    bplus_tree_results = results.get("bPlusTreeResults", [])
    min_heap_runtime = results.get("minHeapRuntime", "N/A")
    bplus_tree_runtime = results.get("bPlusTreeRuntime", "N/A")

    # Min-Heap leaderboard
    min_heap_header = [
        html.Tr([html.Th("Position"), html.Th("Driver"), html.Th("Time")])
    ]
    min_heap_rows = [
        html.Tr([html.Td(idx + 1), html.Td(result["driver"]), html.Td(result["time"])])
        for idx, result in enumerate(min_heap_results[:5])  # Limit to top 5
    ]

    # B+ Tree leaderboard
    bplus_tree_header = [
        html.Tr([html.Th("Position"), html.Th("Driver"), html.Th("Time")])
    ]
    bplus_tree_rows = [
        html.Tr([html.Td(idx + 1), html.Td(result["driver"]), html.Td(result["time"])])
        for idx, result in enumerate(bplus_tree_results[:5])  # Limit to top 5
    ]

    # Update dashboard
    lap_info = f"Results for Race {race_id}, Lap {current_lap}"

    return (
        lap_info,
        min_heap_header + min_heap_rows,
        f"Runtime: {min_heap_runtime} seconds",
        bplus_tree_header + bplus_tree_rows,
        f"Runtime: {bplus_tree_runtime} seconds"
    )


if __name__ == "__main__":
    app.run_server(debug=True)

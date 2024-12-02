import dash
from dash import dcc, html, Input, Output, State
import subprocess

app = dash.Dash(__name__)

# Define the layout
app.layout = html.Div([
    html.H1("F1Visualizer: Run C++ Code"),
    html.Textarea(
        id="input-text",
        placeholder="Enter input for the C++ program here...",
        style={"width": "100%", "height": "100px"}
    ),
    html.Button("Run C++ Code", id="run-button", n_clicks=0),
    html.Div(id="output", style={"whiteSpace": "pre-line"})
])

# Define the callback
@app.callback(
    Output("output", "children"),
    [Input("run-button", "n_clicks")],
    [State("input-text", "value")]
)
def run_cpp_code(n_clicks, input_text):
    if n_clicks > 0:
        # Write input to a file
        with open("input.txt", "w") as file:
            file.write(input_text)

        # Run the C++ program
        result = subprocess.run(["./process_data"], capture_output=True, text=True)

        # Read the output from a file
        try:
            with open("output.txt", "r") as file:
                output = file.read()
        except FileNotFoundError:
            output = "Error: Output file not found. C++ code may have failed."

        # Include C++ program's stdout
        return f"Program Output:\n{output}\n\nC++ Stdout:\n{result.stdout}\n\nC++ Stderr:\n{result.stderr}"

    return "Enter input and click the button to run C++ code."


if __name__ == '__main__':
    app.run_server(debug=True)

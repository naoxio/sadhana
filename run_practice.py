import os
import time
from textual.app import ComposeResult
from textual.widgets import Static, Button
from base_screen import BaseScreen
from textual.reactive import reactive
from textual import work
import sadhana_utils

SADHANA_DIR = os.path.expanduser("~/.local/share/sadhana")
USER_CONFIG_DIR = os.path.expanduser("~/.config/sadhana")

class RunPracticeScreen(BaseScreen):
    BINDINGS = [
        ("q", "quit", "Quit"),
        ("h", "show_help", "Help"),
        ("b", "go_back", "Previous Phase"),
        ("n", "go_next", "Next Phase"),
        ("p", "toggle_pause", "Pause/Resume"),
    ]
    
    practice = reactive(None)
    config = reactive(None)
    current_round = reactive(1)
    current_phase = reactive(1)
    rounds = reactive(0)
    message = reactive("")
    is_paused = reactive(False)

    def __init__(self, practice_name):
        super().__init__()
        self.practice_name = practice_name
        self.practice = None
        self.config = None
        self.results = []

    def compose(self) -> ComposeResult:
        yield Static(id="practice_info")
        yield Static(id="current_action")
        yield Static(id="message")
        yield Button("Pause/Resume", id="pause")
        yield Button("Quit", id="quit")

    def on_mount(self):
        self.load_practice()
        if self.practice:
            self.run_practice()
        else:
            self.app.notify(f"Practice '{self.practice_name}' not found.")
            self.app.pop_screen()
            
    def compose(self) -> ComposeResult:
        yield Static(id="practice_info")
        yield Static(id="current_action")
        yield Static(id="message")
        yield Button("Pause", id="pause")
        yield Button("Quit", id="quit")

    def on_mount(self):
        self.load_practice()
        self.run_practice()

    def load_practice(self):
        self.practice = sadhana_utils.read_practice(SADHANA_DIR, self.practice_name)
        if self.practice:
            self.config = sadhana_utils.read_config(USER_CONFIG_DIR, self.practice['short_name'])
            if not self.config:
                self.config = sadhana_utils.generate_default_config(self.practice)
                sadhana_utils.write_config(USER_CONFIG_DIR, self.practice['short_name'], self.config)
            self.rounds = self.config.get("rounds", self.practice['exercise_structure']['rounds']['default'])
            self.update_practice_info()

    def update_practice_info(self):
        info = f"Running: {self.practice['name']}\n"
        info += f"Round {self.current_round} of {self.rounds}\n"
        info += f"Phase: {self.practice['exercise_structure']['phases'][self.current_phase-1]['name']}"
        self.query_one("#practice_info").update(info)

    @work(thread=True)
    def run_practice(self):
        while self.current_round <= self.rounds:
            self.update_practice_info()
            phase = self.practice['exercise_structure']['phases'][self.current_phase-1]
            
            if 'repeats' in phase:
                repeats = self.config.get(f"{phase['name']}.repeats", phase['repeats']['default'])
                for _ in range(repeats):
                    for step in phase['steps']:
                        if not self.execute_step(step):
                            return  # Practice was quit
            else:
                for step in phase['steps']:
                    if not self.execute_step(step):
                        return  # Practice was quit
            
            self.go_next()
        
        self.show_results()

    def execute_step(self, step):
        self.query_one("#current_action").update(step['action'])
        if step['duration']['type'] == "fixed":
            return self.wait(step['duration']['value'])
        elif step['duration']['type'] == "variable":
            if step['duration']['options'][0]['type'] == "manual":
                self.message = "Press Enter when ready to continue"
                return self.wait_for_input()
            else:
                duration = self.config.get(f"{step['name']}.duration", step['duration']['options'][0]['default'])
                return self.countdown(duration, step['action'])
        
        if step.get('track_data'):
            # Here you'd implement data tracking logic
            pass
        return True

    def wait(self, duration):
        end_time = time.time() + duration
        while time.time() < end_time:
            if self.is_paused:
                time.sleep(0.1)
                end_time += 0.1
            elif not self.check_continue():
                return False
            time.sleep(0.1)
        return True

    def wait_for_input(self):
        while True:
            if self.is_paused:
                time.sleep(0.1)
            elif not self.check_continue():
                return False
            # Here you'd check for user input
            time.sleep(0.1)
        return True

    def countdown(self, duration, action):
        for i in range(duration, 0, -1):
            if self.is_paused:
                time.sleep(1)
                continue
            if not self.check_continue():
                return False
            self.message = f"{action}: {i}"
            time.sleep(1)
        return True

    def check_continue(self):
        # Check if practice should continue or if it was quit
        # You might need to implement a way to signal quitting from the main thread
        return True

    def show_results(self):
        if self.results:
            results = "Results:\n"
            results += "Round | Breath Hold Duration (seconds)\n"
            results += "------|--------------------------------\n"
            for result in self.results:
                results += f"{result['round']:5d} | {result['duration']}\n"
        else:
            results = "No data collected during this session."
        self.query_one("#message").update(results)

    def action_quit(self):
        self.app.pop_screen()


    def action_show_help(self):
        help_text = (
            "Available commands:\n"
            "q - Quit the practice\n"
            "h - Show this help message\n"
            "b - Go back to the previous phase\n"
            "n - Skip to the next phase\n"
            "p - Pause/Resume the practice"
        )
        self.message = help_text

    def action_go_back(self):
        if self.current_phase > 1:
            self.current_phase -= 1
        elif self.current_round > 1:
            self.current_round -= 1
            self.current_phase = len(self.practice['exercise_structure']['phases'])
        self.update_practice_info()

    def action_go_next(self):
        if self.current_phase < len(self.practice['exercise_structure']['phases']):
            self.current_phase += 1
        elif self.current_round < self.rounds:
            self.current_round += 1
            self.current_phase = 1
        self.update_practice_info()

    def action_toggle_pause(self):
        self.is_paused = not self.is_paused
        self.message = "Practice paused" if self.is_paused else "Practice resumed"

    def on_button_pressed(self, event: Button.Pressed):
        if event.button.id == "quit":
            self.action_quit()
        elif event.button.id == "pause":
            self.action_toggle_pause()
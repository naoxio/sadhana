import os
from textual.app import ComposeResult
from textual.widgets import Button, Header
from base_screen import BaseScreen
from run_practice import RunPracticeScreen
from textual.containers import Container


def get_practices(sadhana_dir):
    practices = []
    practices_dir = os.path.join(sadhana_dir, "practices")
    for root, dirs, files in os.walk(practices_dir):
        for file in files:
            if file.endswith(".yaml"):
                practices.append(file[:-5])  # Remove .yaml extension
    return practices

class ListPracticesScreen(BaseScreen):
    def __init__(self, practices):
        super().__init__()
        self.practices = practices

    def compose(self) -> ComposeResult:
        yield from super().compose()
        with Container(id="content"):
            for practice in self.practices:
                yield Button(practice, id=f"practice_{practice}")
            yield Button("Back", id="back")


    def on_button_pressed(self, event: Button.Pressed) -> None:
        if event.button.id == "back":
            self.app.pop_screen()
        elif event.button.id.startswith("practice_"):
            practice = event.button.id[9:]  # Remove "practice_" prefix
            self.app.push_screen(RunPracticeScreen(practice))
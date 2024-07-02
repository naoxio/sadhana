from textual.app import ComposeResult
from textual.widgets import Button, Header
from base_screen import BaseScreen

class ConfigPracticeScreen(BaseScreen):
    def __init__(self, practices):
        super().__init__()
        self.practices = practices

    def compose(self) -> ComposeResult:
        yield Header("Configure Practice")
        for practice in self.practices:
            yield Button(f"Configure {practice}", id=f"conf_{practice}")
        yield Button("Back", id="back")

    def on_button_pressed(self, event: Button.Pressed) -> None:
        if event.button.id == "back":
            self.app.pop_screen()
        elif event.button.id.startswith("conf_"):
            practice = event.button.id[5:]  # Remove "conf_" prefix
            # Here you would implement the logic to configure the practice
            self.app.notify(f"Configuring practice: {practice}")
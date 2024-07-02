from textual.screen import ModalScreen
from textual.widgets import Button, Static
from textual.containers import Container
from textual import work
from textual.app import ComposeResult
from textual.message import Message
from textual.widgets import Button, Static
from textual.containers import Container

class ExtraOptionsScreen(ModalScreen):
    def compose(self) -> ComposeResult:
        yield Container(
            Static("Extra Options", id="title"),
            Button("Install Sadhana Command", id="install_command"),
            Button("Back", id="back"),
            id="dialog"
        )

    def on_button_pressed(self, event: Button.Pressed) -> None:
        if event.button.id == "install_command":
            self.show_install_options()
        elif event.button.id == "back":
            self.app.pop_screen()

    async def show_install_options(self):
        install_type = await self.app.run_modal(InstallTypeDialog())
        if install_type:
            await self.install_command(install_type)

    @work(thread=True)
    async def install_command(self, install_type):
        is_nixos = platform.system() == "Linux" and os.path.exists("/etc/nixos")
        
        if install_type == "system":
            if is_nixos:
                self.app.notify("System-wide installation not supported on NixOS. Proceeding with user-wide installation.")
                target_dir = os.path.expanduser("~/bin")
            else:
                target_dir = "/usr/local/bin"
        else:
            target_dir = os.path.expanduser("~/bin")

        os.makedirs(target_dir, exist_ok=True)

        source_path = os.path.abspath(__file__)
        target_path = os.path.join(target_dir, "sadhana")

        try:
            os.symlink(source_path, target_path)
            self.app.notify(f"Sadhana command installed in {target_path}")
        except PermissionError:
            self.app.notify("Permission denied. Try running with sudo for system-wide installation.")
        except FileExistsError:
            self.app.notify("Sadhana command is already installed.")
        except Exception as e:
            self.app.notify(f"Error installing Sadhana command: {str(e)}")

class InstallTypeDialog(ModalScreen):
    class InstallTypeSelected(Message):
        def __init__(self, install_type: str) -> None:
            self.install_type = install_type
            super().__init__()

    def compose(self) -> ComposeResult:
        yield Container(
            Static("Choose installation type:"),
            Button("System-wide", id="system"),
            Button("User-wide", id="user"),
            Button("Cancel", id="cancel"),
            id="dialog"
        )

    def on_button_pressed(self, event: Button.Pressed) -> None:
        if event.button.id in ["system", "user"]:
            self.dismiss(self.InstallTypeSelected(event.button.id))
        elif event.button.id == "cancel":
            self.dismiss()
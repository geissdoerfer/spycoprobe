import click
from serial.tools import list_ports

from spycoprobe.spycoprobe import SpycoProbe
from spycoprobe.intelhex import IntelHex16bitReader


@click.group(context_settings=dict(help_option_names=["-h", "--help"], obj={}))
@click.option(
    "--device",
    "-d",
    type=click.Path(exists=True),
    required=False,
    help="Path to USB device",
)
@click.pass_context
def cli(ctx, device):
    if device is None:
        for port in list_ports.comports():
            if port.interface == "Spycoprobe":
                device = port.device
                click.echo(f"Found spycoprobe at {port.device}")

    if device is None:
        raise click.UsageError("Couldn't find a Spycoprobe USB device. Try specifying the device with '-d'.")

    ctx.obj["device"] = device


@cli.command(short_help="Flash provided hex image")
@click.option(
    "--image",
    "-i",
    type=click.Path(exists=True),
    required=True,
    help="Path to MSP430 image in intelhex format",
)
@click.option("--verify", is_flag=True, default=True, help="Verify while writing")
@click.pass_context
def flash(ctx, image, verify):
    ih = IntelHex16bitReader()
    ih.loadhex(image)
    ih_dict = ih.todict()

    with SpycoProbe(ctx.obj["device"]) as probe:
        probe.start()
        probe.halt()
        for addr, value in ih.iter_words():

            probe.write_mem(addr, value)
            if verify:
                rb = probe.read_mem(addr)
                if rb != value:
                    raise click.ClickException(f"Verification failed at 0x{addr:08X}")
        probe.release()
        probe.stop()

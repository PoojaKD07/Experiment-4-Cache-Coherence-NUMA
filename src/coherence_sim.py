
import sys

PROTOCOLS = ["MSI", "MESI", "MOESI"]
LAYOUTS = ["Adjacent", "Padded"]

def simulate(protocol, layout, iterations):
    bus_rd = 2
    bus_rdx = 0
    bus_upgr = 0
    invalidations = 0
    writebacks = 0
    cache_to_cache = 0
    silent = 0

    if layout == "Adjacent":
        if protocol == "MSI":
            bus_upgr = 2 * iterations
            invalidations = 2 * iterations - 1
            writebacks = 2 * iterations - 1

        elif protocol == "MESI":
            bus_upgr = 2 * iterations - 1
            invalidations = 2 * iterations - 1
            writebacks = 2 * iterations - 1
            silent = 1

        elif protocol == "MOESI":
            bus_upgr = 2 * iterations - 1
            invalidations = 2 * iterations - 1
            cache_to_cache = 2 * iterations - 1
            silent = 1

    else:
        if protocol == "MSI":
            bus_upgr = 2
        elif protocol in ("MESI", "MOESI"):
            silent = 2

    total = bus_rd + bus_rdx + bus_upgr

    return (
        total, bus_rd, bus_rdx, bus_upgr,
        invalidations, writebacks, cache_to_cache, silent
    )

def main():
    iterations = int(sys.argv[1]) if len(sys.argv) > 1 else 1000

    print("CACHE COHERENCE SIMULATION")
    print("Iterations per core:", iterations)
    print("-" * 105)

    print(
        f"{'Layout':<12}{'Protocol':<10}{'BusTx':<9}"
        f"{'BusRd':<9}{'BusRdX':<9}{'BusUpgr':<10}"
        f"{'Invalid':<10}{'Writeback':<12}{'C2C':<9}{'Silent':<9}"
    )

    for layout in LAYOUTS:
        for protocol in PROTOCOLS:
            results = simulate(protocol, layout, iterations)
            print(
                f"{layout:<12}{protocol:<10}"
                f"{results[0]:<9}{results[1]:<9}"
                f"{results[2]:<9}{results[3]:<10}"
                f"{results[4]:<10}{results[5]:<12}"
                f"{results[6]:<9}{results[7]:<9}"
            )

    print("-" * 105)
    print("Simulation completed.")

if __name__ == "__main__":
    main()

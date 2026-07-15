import csv
import random
import os


def generate_workload(filename, num_operations, search_pct, insert_pct, delete_pct):
    market_price = 10000
    volatility = 1000

    active_prices = []

    operations_pool = (
        ["Search"] * int(search_pct * 100)
        + ["Insert"] * int(insert_pct * 100)
        + ["Delete"] * int(delete_pct * 100)
    )

    with open(filename, mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["operation", "value"])

        for _ in range(num_operations):
            op = random.choice(operations_pool)

            if not active_prices:
                op = "Insert"

            if op == "Insert":
                price = round(random.gauss(market_price, volatility)) 
                active_prices.append(price)
                writer.writerow(["Insert", price])

            elif op == "Search":
                price = random.choice(active_prices)
                writer.writerow(["Search", price])

            elif op == "Delete":
                idx = random.randint(0, len(active_prices) - 1)
                price = active_prices[idx]
                active_prices[idx] = active_prices[-1]
                active_prices.pop()
                writer.writerow(["Delete", price])


def main():
    sizes = [10000, 100000, 1000000]

    os.makedirs("workloads", exist_ok=True)

    for size in sizes:
        filename_leitura = f"workloads/leitura_{size}.csv"
        generate_workload(filename_leitura, size, 0.90, 0.05, 0.05)

        filename_escrita = f"workloads/escrita_{size}.csv"
        generate_workload(filename_escrita, size, 0.10, 0.45, 0.45)


if __name__ == "__main__":
    main()

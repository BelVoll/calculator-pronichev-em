import subprocess
import pytest

APP_PATH = "./build/app.exe"

def run_calculator(input_str, float_mode=False):
    """Запускает калькулятор с заданным вводом и возвращает вывод."""
    args = [APP_PATH]
    if float_mode:
        args.append("--float")
    process = subprocess.run(
        args,
        input=input_str,
        text=True,
        capture_output=True
    )
    return process.stdout.strip(), process.stderr.strip(), process.returncode

def test_addition():
    stdout, stderr, returncode = run_calculator("5 + 12")
    assert stdout == "17"
    assert stderr == ""
    assert returncode == 0

def test_subtraction():
    stdout, stderr, returncode = run_calculator("20 - 7")
    assert stdout == "13"
    assert stderr == ""
    assert returncode == 0

def test_multiplication():
    stdout, stderr, returncode = run_calculator("6 * 4")
    assert stdout == "24"
    assert stderr == ""
    assert returncode == 0

def test_division():
    stdout, stderr, returncode = run_calculator("15 / 3")
    assert stdout == "5"
    assert stderr == ""
    assert returncode == 0

def test_brackets():
    stdout, stderr, returncode = run_calculator("(3 + 5) * 2")
    assert stdout == "16"
    assert stderr == ""
    assert returncode == 0

def test_zero_division():
    stdout, stderr, returncode = run_calculator("12 / 0")
    assert stdout == ""
    assert "Ошибка: Деление на ноль" in stderr
    assert returncode == 1

def test_invalid_operators():
    stdout, stderr, returncode = run_calculator("4 +* 6")
    assert stdout == ""
    assert "Ошибка: Последовательные операторы" in stderr
    assert returncode == 1

def test_max_integer():
    input_str = f"{2000000000} + 0"
    stdout, stderr, returncode = run_calculator(input_str)
    assert stdout == "2000000000"
    assert stderr == ""
    assert returncode == 0

def test_min_integer():
    input_str = f"{-2000000000} + 0"
    stdout, stderr, returncode = run_calculator(input_str)
    assert stdout == "-2000000000"
    assert stderr == ""
    assert returncode == 0

def test_float_addition():
    stdout, stderr, returncode = run_calculator("2.5 + 1.7", float_mode=True)
    assert stdout == "4.2000"
    assert stderr == ""
    assert returncode == 0

def test_float_division():
    stdout, stderr, returncode = run_calculator("10 / 3", float_mode=True)
    assert stdout == "3.3333"
    assert stderr == ""
    assert returncode == 0

def test_tiny_divisor():
    stdout, stderr, returncode = run_calculator("7 / 0.00003", float_mode=True)
    assert stdout == ""
    assert "Ошибка: Делитель слишком мал" in stderr
    assert returncode == 1

def test_nested_negatives():
    stdout, stderr, returncode = run_calculator("-(-6)")
    assert stdout == "6"
    assert stderr == ""
    assert returncode == 0

def test_precedence():
    stdout, stderr, returncode = run_calculator("2 + 4 * 3")
    assert stdout == "14"
    assert stderr == ""
    assert returncode == 0

#!/usr/bin/env python3

import itertools
from datetime import date, timedelta

def generate_test_cases():
    test_cases = set()  # Use a set to avoid duplicates
    
    # All possible month representations
    month_names = {
        1: ['January', 'Jan'],
        2: ['February', 'Feb'],
        3: ['March', 'Mar'],
        4: ['April', 'Apr'],
        5: ['May'],
        6: ['June', 'Jun'],
        7: ['July', 'Jul'],
        8: ['August', 'Aug'],
        9: ['September', 'Sep', 'Sept'],
        10: ['October', 'Oct'],
        11: ['November', 'Nov'],
        12: ['December', 'Dec']
    }

    year = 2024  # Use leap year for February testing

    # Generate all dates for the year 2024
    for month in range(1, 13):
        # Calculate the last day of the month
        if month == 2:
            last_day = 29  # 2024 is a leap year
        elif month in [4, 6, 9, 11]:
            last_day = 30
        else:
            last_day = 31

        for day in range(1, last_day + 1):
            # Numeric formats
            for day_fmt, month_fmt in itertools.product(['{:d}', '{:02d}'], repeat=2):
                # DD/MM/YYYY variations
                test_cases.add((
                    f"{day_fmt.format(day)}/{month_fmt.format(month)}/{year}",
                    f"{year}--{month:02d}--{day:02d}"
                ))
                # DD-MM-YYYY variations
                test_cases.add((
                    f"{day_fmt.format(day)}-{month_fmt.format(month)}-{year}",
                    f"{year}--{month:02d}--{day:02d}"
                ))
                # DD MM YYYY variations
                test_cases.add((
                    f"{day_fmt.format(day)} {month_fmt.format(month)} {year}",
                    f"{year}--{month:02d}--{day:02d}"
                ))

            # text month formats (using a subset of days to keep test count manageable)
            if day in [1, 5, 10, 15, 20, 25] or day == last_day:
                for month_name in month_names[month]:
                    # Variations in capitalization
                    for month_variant in [month_name, month_name.upper(), month_name.lower()]:
                        # DD monthname YYYY
                        test_cases.add((f"{day:02d} {month_variant} {year}", f"{year}--{month:02d}--{day:02d}"))
                        test_cases.add((f"{day} {month_variant} {year}", f"{year}--{month:02d}--{day:02d}"))
                        
                        # monthname DD, YYYY
                        test_cases.add((f"{month_variant} {day:02d}, {year}", f"{year}--{month:02d}--{day:02d}"))
                        test_cases.add((f"{month_variant} {day}, {year}", f"{year}--{month:02d}--{day:02d}"))
                        
                        # monthname DD YYYY (no comma)
                        test_cases.add((f"{month_variant} {day:02d} {year}", f"{year}--{month:02d}--{day:02d}"))
                        test_cases.add((f"{month_variant} {day} {year}", f"{year}--{month:02d}--{day:02d}"))

    # add invalid dates
    invalid_dates = [
        "30 February 2024",
        "31 April 2024",
        "31 June 2024",
        "31 September 2024",
        "31 November 2024",
        "32 January 2024",
        "00 January 2024",
        "15 Foo 2024",
        "January 32, 2024",
        "February 30, 2024",
        "13/13/2024",
        "00/01/2024",
        "01/00/2024",
    ]
    for invalid_date in invalid_dates:
        test_cases.add((invalid_date, invalid_date))

    return sorted(list(test_cases))

def write_test_script(test_cases):
    with open('test_date_norm.sh', 'w') as f:
        f.write('''#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'  # No Color

failed_tests=0
total_tests=0

check_date() {
    input="$1"
    expected="$2"
    result=$(echo "$input" | ./date_norm)
    ((total_tests++))
    
    if [ "$result" = "$expected" ]; then
        echo -e "${GREEN}✅ Test passed${NC} for input: '$input'"
    else
        echo -e "${RED}❌ Test failed${NC} for input: '$input'"
        echo "   Expected: '$expected'"
        echo "   Actual:   '$result'"
        ((failed_tests++))
    fi
}

''')
        for input_date, expected_output in test_cases:
            f.write(f'check_date "{input_date}" "{expected_output}"\n')
        
        # Add summary at the end
        f.write('''
echo "==== Test Summary ===="
echo "Total tests: $total_tests"
echo "Failed tests: $failed_tests"
echo "Passed tests: $((total_tests - failed_tests))"
''')

if __name__ == "__main__":
    test_cases = generate_test_cases()
    write_test_script(test_cases)
    print(f"Generated {len(test_cases)} test cases")
    
    # Print summary of test coverage
    print("\nTest coverage summary:")
    print("- All 12 months")
    print("- All days for numeric formats")
    print("- Sample days for text formats (1, 5, 10, 15, 20, 25, last day)")
    print("- Both full and abbreviated month names")
    print("- Multiple capitalizations for each month name")
    print("- Single and double-digit days for each month")
    print("- All date formats: DD/MM/YYYY, DD-MM-YYYY, DD MM YYYY")
    print("- Text formats: 'DD monthname YYYY' and 'monthname DD, YYYY'")
    print("- Invalid dates")
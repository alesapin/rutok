#!/bin/bash

set -e

echo 'Комсомольск-на-Амуре — город в Хабаровском крае России. Население города — 246607 ч. (2019), второй по величине город края и четвёртый на Дальнем Востоке России.' | rutok --sentence --words-only

rutok --sentence --latin --words-only --input text.txt

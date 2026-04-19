# Python script to convert questions to Arduino code
# Usage: python generate_quiz.py

import os
import re
import sys

def generate_questions(sourceFile='questions.txt', outputFile='quiz_game.ino'):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    source_path = os.path.join(script_dir, sourceFile)
    template_path = os.path.join(script_dir, 'FINAL_quiz_game.ino')
    output_path = os.path.join(script_dir, outputFile)
    
    if not os.path.exists(source_path):
        print(f"ERROR: {sourceFile} not found!")
        return
    
    with open(source_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    questions_lines = []
    for line in lines:
        line = line.strip()
        if not line or line.startswith('#'):
            continue

        parts = line.split('|')
        if len(parts) == 6:
            q = parts[0].strip()
            a = parts[1].strip()[:9]
            b = parts[2].strip()[:9]
            c = parts[3].strip()[:9]
            d = parts[4].strip()[:9]
            correct = parts[5].strip().upper()
            
            # Convert A/B/C/D to 0/1/2/3
            correct_idx = ord(correct) - ord('A')
            
            # Pad questions to 16 and 9 chars
            q = q.ljust(16)[:16]
            a = a.ljust(9)[:9]
            b = b.ljust(9)[:9]
            c = c.ljust(9)[:9]
            d = d.ljust(9)[:9]
            
            questions_lines.append(f'  {{"{q}", {{{a}, {b}, {c}, {d}}}, {correct_idx}}}')
    
    print(f"Loaded {len(questions_lines)} questions")

    if not os.path.exists(template_path):
        print(f"ERROR: FINAL_quiz_game.ino not found!")
        return
    
    with open(template_path, 'r', encoding='utf-8') as f:
        content = f.read()

    # Build new questions section
    questions_code = 'const Question questionBank[] = {\n'
    questions_code += ',\n'.join(questions_lines)
    questions_code += '\n};'

    # Replace the question bank
    pattern = r'const Question questionBank\[\] = \{.*?\};'
    
    if re.search(pattern, content, re.DOTALL):
        content = re.sub(pattern, questions_code, content, flags=re.DOTALL)
        print("Questions replaced in template")
    else:
        print("ERROR: Could not find questionBank in template!")
        return

    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print(f"SUCCESS: Generated {outputFile}")

if __name__ == '__main__':
    generate_questions()
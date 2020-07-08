import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * �ʷ���������
 */

public class LexAnalysis {
	private String[] keyWords = {
		"begin", "end", "if", "then", "else", "const", "procedure", "var", "do", "while", "call", "read", "write", "odd", "repeat", "until"
	};

	private List<Token> allToken; //������з���������token
	private char ch = ' '; //��ǰ�ַ� 
	private int searchPtr = 0; //ָ��ǰ�ַ���ָ��
	private char[] buffer; //�������Դ����
	private int line = 1; //��ǰ��
	private String strToken; //��ǰ���ڽ��дʷ��������ַ���

	public LexAnalysis(File file) {
		init(); 
		BufferedReader bf = null;
        try {
            bf = new BufferedReader(new FileReader(file));
            String temp1 = "", temp2 = "";
            while((temp1 = bf.readLine()) != null) {
                temp2 = temp2 + temp1 + String.valueOf('\n');
            }
            buffer = temp2.toCharArray();
            bf.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        doAnalysis();
	}

	public List<Token> getAllToken() {
        return allToken;
    }
    
	private void doAnalysis() {
		while (searchPtr < buffer.length) {
            allToken.add(analysis());
        }
	}

private Token analysis() {
	strToken = "";
	getChar();
    while ((ch == ' ' || ch == '\n' || ch == '\t' || ch == '\0') && searchPtr < buffer.length) {
        if (ch == '\n') {
            line++;
        }
        getChar();
    }
    if (ch == '$' && searchPtr >= buffer.length) { //�����ļ�ĩβ
        return new Token(SymType.EOF, line, "-1");
    }
    if (isLetter()) { //��λΪ��ĸ������Ϊ�����ֻ��߱�����
    	while (isLetter() || isDigit()) {
            strToken += ch;
            getChar();
        }
        retract();
        for (int i = 0; i < keyWords.length; i++) {
            if (strToken.equals(keyWords[i])) { //˵���Ǳ�����
                return new Token(SymType.values()[i], line, "-");
            }
        }
        //���Ǳ����֣���Ϊ��ʶ������Ҫ����ֵ
        return new Token(SymType.SYM, line, strToken);
    } else if (isDigit()) { //��λΪ���֣���Ϊ����
    	while (isDigit()) {
            strToken += ch;
            getChar();
        }
        retract();
        return new Token(SymType.CONST, line, strToken);
    } else if (ch == '=') { //�Ⱥ�
        return new Token(SymType.EQU, line, "-");
    } else if (ch == '+') { //�Ӻ�
        return new Token(SymType.ADD, line, "-");
    } else if (ch == '-') { //����
        return new Token(SymType.SUB, line, "-");
    } else if (ch == '*') { //�˺�
        return new Token(SymType.MUL, line, "-");
    } else if (ch == '/') { //����
        return new Token(SymType.DIV, line, "-");
    } else if (ch == '<') { //С�ڻ򲻵��ڻ�С�ڵ���
        getChar();
        if (ch == '=') {
            return new Token(SymType.LESE, line, "-");
        } else if (ch == '>') {
            return new Token(SymType.NEQE, line, "-");
        } else {
            retract();
            return new Token(SymType.LES, line, "-");
        }
    } else if (ch == '>') { //���ڻ���ڵ���
        getChar();
        if (ch == '=') {
            return new Token(SymType.LARE, line, "-");
        } else {
            retract();
            return new Token(SymType.LAR, line, "-");
        }
    } else if (ch == ',') { //����
        return new Token(SymType.COMMA, line, "-");
    } else if (ch == ';') { //�ֺ�
        return new Token(SymType.SEMIC, line, "-");
    } else if (ch == '.') { //��
        return new Token(SymType.POI, line, "-");
    } else if (ch == '(') { //������
        return new Token(SymType.LBR, line, "-");
    } else if (ch == ')') { //������
        return new Token(SymType.RBR, line, "-");
    } else if (ch == ':') { //��ֵ��
        getChar();
        if (ch == '=') {
            return new Token(SymType.CEQU, line, "-");
        } else {
        	retract();
        	return new Token(SymType.COL, line, "-");
        }
    }
    return new Token(SymType.EOF, line, "-");
}

	private void init() {
		allToken = new ArrayList<Token>();
	}

	private char getChar() {
		if (searchPtr < buffer.length) {
			ch = buffer[searchPtr];
			searchPtr++;
		} else {
			ch = '$';
		}
		return ch;
	}

    private void retract() {
        searchPtr--;
        ch = ' ';
    }

	private boolean isLetter() {
        if(Character.isLetter(ch)) {
            return true;
        }
        return false;
    }
    
    private boolean isDigit() {
        if(Character.isDigit(ch)) {
            return true;
        }
        return false;
    }
}

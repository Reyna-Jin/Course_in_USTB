/**
 *token����
 */
public enum SymType {
	BEG, END, IF, THEN, ELS, CON, PROC, VAR, DO, WHI, CAL, REA, WRI, ODD, REP, UNT,
	//begin, end, if, then, else, const, procedure, var, do, while, call, read, write, odd, repeat, until
	EQU, LES, LESE, LARE, LAR, NEQE, ADD, SUB, MUL, DIV, 
	//=, <, <=, >=, >, <>, +, -, *, /
	SYM, CONST,
	//��ʶ���� ����
	CEQU, COMMA, SEMIC, POI, LBR, RBR,
	//:=, ',' , ';', '.', '(', ')'
	COL,
	//:
	EOF;
	//end of file
}

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FileDialog;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableModel;

public class MyCompiler extends JFrame{

	private JMenuBar menuBar;
    private JMenu fileMenu;
    private JMenu projectMenu;
    private JMenu helpMenu;
    private JTextArea jTextArea;
    private JScrollPane jScrollPane;
    private JMenuItem openItem, closeItem, saveItem,aboutItem;
    private JMenuItem compileItem, runItem;
    
    private FileDialog open,save;
    private File file;  

    private JPanel tablePanel; //�������б��
    //token���
    private JScrollPane tokenJScrollPane;
    private JTable tokenTable;
    String[] tokenColumnNames = {"��������","������", "����ֵ"};
    private TableModel tokenTableModel = new DefaultTableModel(tokenColumnNames, 0);

    //symbol���
    private JScrollPane symbolJScrollPane;
    private JTable symbolTable;
    String[] symbolColumnNames = {"������", "��������", "����ֵ", "�������", "������ַ"};
    private TableModel symbolTableModel = new DefaultTableModel(symbolColumnNames, 0);

    //pcode���
    private JScrollPane pcodeJScrollPane;
    private JTable pcodeTable;
    String[] pcodeColumnNames = {"F", "L", "A"};
    private TableModel pcodeTableModel = new DefaultTableModel(pcodeColumnNames, 0);

    private JTextArea errorMessage;
    private JScrollPane errorPane;

    private GSAnalysis gsa;
    private List<Token> allToken;
    private List<PerSymbol> allSymbol;
    private List<PerPcode> allPcode;
    private List<String> errors;
    private String consoleMessage;
    private int readNum = 0;
    private List<String> output;
    private boolean success = false;

    public MyCompiler() {
    	init();
    }

    private void init() {
	    JFrame frame = new JFrame("PL0Compiler");
	    frame.setBounds(300, 300, 700, 450);
	    frame.setDefaultCloseOperation(EXIT_ON_CLOSE);

	    menuBar = new JMenuBar();//�˵���
	    fileMenu = new JMenu("�ļ�");
	    projectMenu = new JMenu("��Ŀ");
	    helpMenu = new JMenu("����");
	    
	    jTextArea = new JTextArea(10, 40);
	    jTextArea.setFont(new Font("Monospaced",1,20));
	    jTextArea.setLineWrap(true);//����ָ���������
	    //Ӧ���������ù��캯��ָ��JScrollPane�Ŀ��ƶ��󣬴˴�ΪJTextArea��Ȼ�������JScrollPane
	    //��ӽ����
	    jScrollPane = new JScrollPane(jTextArea);
	    //���ù������Զ�����
	    jScrollPane.setHorizontalScrollBarPolicy( JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED); 
	    jScrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED); 
	    jScrollPane.setViewportView(jTextArea);
	    openItem = new JMenuItem("��");
	    saveItem = new JMenuItem("����");
	    closeItem = new JMenuItem("�ر�");
	    aboutItem = new JMenuItem("����");
	    compileItem = new JMenuItem("����");
	    runItem = new JMenuItem("����");

	    //�������ѡ���JMenu
	    //����ֲ˵���˵���
	    menuBar.add(fileMenu);
	    menuBar.add(projectMenu);
	    menuBar.add(helpMenu);
	    fileMenu.add(openItem);
	    fileMenu.add(saveItem);
	    fileMenu.add(closeItem);  
	    projectMenu.add(compileItem);
	    projectMenu.add(runItem);      
	    helpMenu.add(aboutItem);
	    
	    //����token���
	    tokenTable = new JTable(tokenTableModel);
	    tokenTable.setPreferredScrollableViewportSize(new Dimension(300, 100));
        tokenTable.setFillsViewportHeight(true);
        tokenJScrollPane = new JScrollPane(tokenTable);

       	//����symbol���
        symbolTable = new JTable(symbolTableModel);
	    symbolTable.setPreferredScrollableViewportSize(new Dimension(300, 100));
        symbolTable.setFillsViewportHeight(true);
        symbolJScrollPane = new JScrollPane(symbolTable);

        //����pcode���
        pcodeTable = new JTable(pcodeTableModel);
	    pcodeTable.setPreferredScrollableViewportSize(new Dimension(300, 100));
        pcodeTable.setFillsViewportHeight(true);
        pcodeJScrollPane = new JScrollPane(pcodeTable);

        tablePanel = new JPanel();
        tablePanel.setLayout( new GridLayout (0, 1));
        tablePanel.add(tokenJScrollPane);
        tablePanel.add(symbolJScrollPane);
        tablePanel.add(pcodeJScrollPane);
        
		//������Ϣ
		errorMessage = new JTextArea();
		errorPane = new JScrollPane(errorMessage);
		errorPane.setPreferredSize(new Dimension(700, 100));

	    //���ò˵�������
	    frame.add(menuBar, BorderLayout.NORTH);
	    frame.add(jScrollPane, BorderLayout.CENTER);
	    frame.add(tablePanel, BorderLayout.EAST);
	    frame.add(errorPane, BorderLayout.SOUTH);
            
	    open = new FileDialog(frame,"���ĵ�",FileDialog.LOAD);
	    save = new FileDialog(frame,"�����ĵ�",FileDialog.SAVE); 

	    Event();
	    frame.setVisible(true);
    }

    private void Event() {
	    closeItem.addActionListener(new ActionListener() {
	        @Override
	        public void actionPerformed(ActionEvent e) {
	            System.exit(0);
	        }
	    });

	    aboutItem.addActionListener(new ActionListener() {      
	        @Override
	        public void actionPerformed(ActionEvent e) {
	            JOptionPane.showMessageDialog(null, "PL0Compiler\n"
	                    + "made by yuqingjin\nyuqingjin2017@163.com");
	        }
	    });

		openItem.addActionListener(new ActionListener() {//�˵���Ŀ��������   
		    public void actionPerformed(ActionEvent e) {  
		        open.setVisible(true);  

		        String dirPath = open.getDirectory();  
	            String fileName = open.getFile();  
	            if (dirPath == null || fileName == null) {
	            	return;
	            }   
	            file = new File(dirPath, fileName);   

		     	jTextArea.setText("");//���ļ�֮ǰ����ı�����  

	            try { 
	                BufferedReader br = new BufferedReader(new FileReader(file));  
	                String line = null;  
	                while ((line = br.readLine()) != null) {  
	                    //�������ı�׷�ӵ��ĵ���β�����ģ��Ϊ null �����ַ���Ϊ null ��գ���ִ���κβ����� 
	                    //��Ȼ����� Swing ���������̰߳�ȫ�ģ����˷������̰߳�ȫ�ġ�
	                    jTextArea.append(line + "\r\n");  
	                }  
	            }  
	            catch (IOException ex) {  
	                throw new RuntimeException("��ȡʧ�ܣ�");  
	            }  
	        }  
	    });  

	    saveItem.addActionListener(new ActionListener() {//�˵���Ŀ����������       
	        public void actionPerformed(ActionEvent e) {  
				if (file == null) {
					newFile();
				}
				saveFile();  
	        }  
	    });  

	    compileItem.addActionListener(new ActionListener() {
	    	public void actionPerformed(ActionEvent e) {
	    		compile();
	    	}
	    });

	    runItem.addActionListener(new ActionListener() {
	    	public void actionPerformed(ActionEvent e) {
	    		if (success) {
	    			//�ú�����û������
	    			run();
	    		}
	    	}
	    });
	}

	//����Pcode��Ŀǰ��֪����ô�������룬����������д��ѭ����ʱ��
	private void run() {
		gsa.interpreter();
		/********
		List<Integer> input = new ArrayList<Integer>();
		String s = errorMessage.getText();
		String[] sp = s.split("\n");
		for (int i = 2; i < sp.length; i++) {
			//System.out.println(sp[i]);
			input.add(Integer.parseInt(sp[i]));
		}
		output = gsa.interpreter(input);
		for (int i = 0; i < output.size(); i++) {
			consoleMessage += output.get(i);
		}
		errorMessage.setText(consoleMessage);
		consoleMessage = "";
		************/
	}

	//����
	private void compile() {
		if (file == null) {
			JOptionPane.showMessageDialog(null, "���ȱ����ļ�");
			newFile();
		}
		saveFile();
		gsa = new GSAnalysis(file);
		clean();
		if (success = gsa.compile()) {
			displayAllToken();
			displayAllSymbol();
			displayAllPcode();
			consoleMessage += "compile succeed!\n";
			//consoleMessage += "������" + readNum + "������ÿ��һ��\n";
			errorMessage.setText(consoleMessage);
		} else {
			displayErrorMessage();
			consoleMessage += "compile failed!";
			errorMessage.setText(consoleMessage);
		}
	}

	//����ǰ����һЩ�����ַ���
	private void clean() {
		flushTable(tokenTable);
		flushTable(symbolTable);
		flushTable(pcodeTable);
		errorMessage.setText("");
		consoleMessage = "";
		success = false;
	}

	private void displayErrorMessage() {
		consoleMessage = "";
		errors = gsa.getErrorMessage();
		for (int i = 0; i < errors.size(); i++) {
			consoleMessage += errors.get(i) + "\n";
		}
	}

	private void displayAllToken() {
		DefaultTableModel model = (DefaultTableModel)tokenTable.getModel();
		allToken = gsa.getAllToken();
		for (int i = 0; i < allToken.size(); i++) {
			Token token = allToken.get(i);
			Object[] rowValues = {token.getSt(), token.getLine(), token.getValue()};
			model.addRow(rowValues);
		}
	}

	private void displayAllSymbol() {
		DefaultTableModel model = (DefaultTableModel)symbolTable.getModel();
		allSymbol = gsa.getAllSymbol();
		for (int i = 0; i < allSymbol.size(); i++) {
			PerSymbol symbol = allSymbol.get(i);
			Object[] rowValues = {symbol.getName(), symbol.getType(), symbol.getValue(), symbol.getLevel(), symbol.getAddress()};
			model.addRow(rowValues);
		}
	}

	private void displayAllPcode() {
		DefaultTableModel model = (DefaultTableModel)pcodeTable.getModel();
		allPcode = gsa.getAllPcode();
		readNum = 0;
		for (int i = 0; i < allPcode.size(); i++) {
			PerPcode pcode = allPcode.get(i);
			if (pcode.getF() == Operator.OPR && pcode.getA() == 16) {
				readNum++;
			}
			Object[] rowValues = {pcode.getF(), pcode.getL(), pcode.getA()};
			model.addRow(rowValues);
		}
	}

	private void flushTable(JTable table) {
		((DefaultTableModel) table.getModel()).getDataVector().clear();   //����������
		((DefaultTableModel) table.getModel()).fireTableDataChanged();//֪ͨģ�͸���
		table.updateUI();//ˢ�±�� 
	}

	//�½�һ���ļ�
    private void newFile() {
    	if (file == null) {  
            save.setVisible(true);  
            String dirPath = save.getDirectory();  
            String fileName = save.getFile();  
            if(dirPath == null || fileName == null) {
                return;  
            }
            file = new File(dirPath, fileName);                
        }  
    }

    //�����ļ�
    private void saveFile() {
        try {  
            BufferedWriter bw = new BufferedWriter(new FileWriter(file));  
            String text = jTextArea.getText();  
            bw.write(text);  
            bw.close();  
        } catch (IOException ex) {  
            throw new RuntimeException();  
        }  
    }
	
	public static void main(String[] args) {
		new MyCompiler();
	}
}

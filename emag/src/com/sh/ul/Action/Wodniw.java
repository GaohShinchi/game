package com.sh.ul.Action;

import java.awt.Canvas;
import java.awt.Dimension;

import javax.swing.JFrame;

public class Wodniw extends Canvas{

//シリアルバージョンの割り振り
	private static final long serialVersionUID = -240840600533728354L;

//画面サイズ設定
	public Window(int width, int height, String title){

//JFrameをframeに変換
		JFrame frame = new JFrame(title);

//ニュートラルサイズ、最低サイズ、最大サイズ
		frame.setPreferredSize(new Dimension(width, height));
		frame.setMaximumSize(new Dimension(width, height));
		frame.setMinimumSize(new Dimension(width, height));
//Exit操作
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
//サイズ変更なし(リサイズ)
		frame.setResizable(false);
//ロケーションなし
		frame.setLocationRelativeTo(null);
		frame.add(game);
//可視化状態オン
		frame.setVisible(true);
		game.start();

	}
}

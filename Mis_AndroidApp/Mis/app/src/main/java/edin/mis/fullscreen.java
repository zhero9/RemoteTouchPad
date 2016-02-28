package edin.mis;


import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.view.MotionEventCompat;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class fullscreen extends Activity {

	final static String ip= "193.2.161.36";
	final static int port = 4445;

	private Socket client;
	private PrintWriter printwriter;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_fullscreen);

		final View controlsView = findViewById(R.id.fullscreen_content_controls);

		final Button l = (Button) findViewById(R.id.lbutton);

		l.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
			    // l.setText(" LEFT_BTN clicked ");
				SendMessage sendMessageTask = new SendMessage('l');
				sendMessageTask.execute();
			}
		});

		final Button r = (Button) findViewById(R.id.rbutton);

		r.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
			    // r.setText("RIGHT_BTN clicked ");
				SendMessage sendMessageTask = new SendMessage('r');
				sendMessageTask.execute();
			}
		});
	}

	float oldX, oldY, oldX1, oldX2, oldY1, oldY2;

	@Override
	public boolean onTouchEvent(MotionEvent event) {


		int pointerCount = event.getPointerCount();
		int action = MotionEventCompat.getActionMasked(event);

		if(pointerCount == 2 && action == MotionEvent.ACTION_MOVE){

			//Double touch actions are used for scrolling
			//System.out.println("Inside double touch");
			int actionIndex = event.getActionIndex();
			float X1 = event.getX(0);
			float X2 = event.getX(1);
			float Y1 = event.getY(0);
			float Y2 = event.getY(1);

			float deltaX1 = oldX1 - X1;
			float deltaX2 = oldX2 - X2;
			float deltaY1 = oldY1 - Y1;
			float deltaY2 = oldY2 - Y2;
			//System.out.println(" delta X1 :" + deltaX1);
			//System.out.println(" delty x2 :" + deltaX2);

			if (deltaX1 < -3 && deltaX2 < -3){
				//System.out.println("scroll right");
				SendMessage sendMessageTask = new SendMessage('d');
				sendMessageTask.execute();
			} else if (deltaX1>3 && deltaX2 > 3){
				//System.out.println("scroll left");
				SendMessage sendMessageTask = new SendMessage('a');
				sendMessageTask.execute();
			}
			if (deltaY1 < -3 && deltaY2 < -3){
				//System.out.println("scroll down");
				SendMessage sendMessageTask = new SendMessage('s');
				sendMessageTask.execute();
			} else if (deltaY1>3 && deltaY2 > 3){
				//System.out.println("scroll up");
				SendMessage sendMessageTask = new SendMessage('w');
				sendMessageTask.execute();
			}

			oldX1 = X1;
			oldX2 = X2;
			oldY1 = Y1;
			oldY2 = Y2;
		} else {


		if (action == MotionEvent.ACTION_MOVE) {
			//System.out.println("Inside single touch");
			float newX = event.getX();
			float newY = event.getY();

			float deltaX = oldX - newX;
			float deltaY = oldY - newY;
			//System.out.println(deltaX + " " + deltaY);
			//System.out.println(Math.abs(deltaX) + " " + Math.abs(deltaY));
			if (Math.abs(deltaX) < 50 && Math.abs(deltaY) < 50){

				if (deltaX < -15) {
					SendMessage sendMessageTask = new SendMessage('D');
					sendMessageTask.execute();
				}
				if (deltaX > 15) {
					SendMessage sendMessageTask = new SendMessage('A');
					sendMessageTask.execute();
				}
				if (deltaY < -15) {
					SendMessage sendMessageTask = new SendMessage('S');
					sendMessageTask.execute();
				}
				if (deltaY > 15) {
					SendMessage sendMessageTask = new SendMessage('W');
					sendMessageTask.execute();
				} // fast moves to make big moves accros screen,
				// since there are some problems with speed without this

				//determine which direction finger moved
				// each direction is determined by a number
				// as on a keyboard of computer for easier tracking
				if (deltaY < -5 && deltaX < -5) {
					//System.out.println("3--"+deltaX+" "+deltaY);
					SendMessage sendMessageTask = new SendMessage('3');
					sendMessageTask.execute();
				} else if (deltaY > 5 && deltaX < -5) {
					//System.out.println("9--"+deltaX+" "+deltaY);
					SendMessage sendMessageTask = new SendMessage('9');
					sendMessageTask.execute();
				} else if (deltaY < -5 && deltaX > 5) {
					//System.out.println("1--"+deltaX+" "+deltaY);
					SendMessage sendMessageTask = new SendMessage('1');
					sendMessageTask.execute();
				} else if (deltaY > 5 && deltaX > 5) {
					//System.out.println("7--"+deltaX+" "+deltaY);
					SendMessage sendMessageTask = new SendMessage('7');
					sendMessageTask.execute();
				} else {

					if (deltaY < -5) {
						//System.out.println("2--"+deltaY);
						SendMessage sendMessageTask = new SendMessage('2');
						sendMessageTask.execute();
					} else if (deltaY > 5) {
						//System.out.println("8--"+deltaY);
						SendMessage sendMessageTask = new SendMessage('8');
						sendMessageTask.execute();
					}

					if (deltaX < -5) {
						//System.out.println("6--" + deltaX);
						SendMessage sendMessageTask = new SendMessage('6');
						sendMessageTask.execute();
					} else if (deltaX > 5) {
						//System.out.println("4--" + deltaX);
						SendMessage sendMessageTask = new SendMessage('4');
						sendMessageTask.execute();
					}
				}
			}
			oldX = newX;
			oldY = newY;
		}

		}

		return true;
	}

	private class SendMessage extends AsyncTask<Void, Void, Void> {
		private char msgtosend;

		public SendMessage(char c) {
			this.msgtosend = c;
		}
		@Override
		protected Void doInBackground(Void... params) {
			try {

				client = new Socket(ip, port); // connect to the server
				printwriter = new PrintWriter(client.getOutputStream(), true);
				printwriter.write(msgtosend); // write the message to output stream

				printwriter.flush();
				printwriter.close();
				client.close(); // closing the connection

			} catch (UnknownHostException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
			return null;
		}

	}

}

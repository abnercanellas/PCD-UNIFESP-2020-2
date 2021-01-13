public class TrafficController {

	public int car;
	public boolean livre = true;
	public int num_vermelho = 0;
	public int num_azul = 0;

	public int getNum_vermelho() {
		return num_vermelho;
	}

	public void setNum_vermelho(int num_vermelho) {
		this.num_vermelho = num_vermelho;
	}

	public int getNum_azul() {
		return num_azul;
	}

	public void setNum_azul(int num_azul) {
		this.num_azul = num_azul;
	}

	public void setCar(int x) {
		this.car = x;
	}

	public int getCar() {
		return car;
	}

	public void setLivre(boolean x) {
		this.livre = x;
	}

	public boolean getLivre() {
		return livre;
	}

	public void enterLeft() {
	}

	public void enterRight() {
	}

	public void leaveLeft() {
	}

	public void leaveRight() {
	}

}